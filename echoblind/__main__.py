import argparse
import dataclasses
import re
from dataclasses import dataclass
from itertools import repeat
from pathlib import Path
from typing import Optional
from xml import sax
from xml.sax.saxutils import XMLFilterBase, XMLGenerator
from xml.sax.xmlreader import AttributesImpl

import pyexcel
from bidict import bidict


@dataclass
class Circuit:
    num: int
    space_num: Optional[int]
    zone_num: Optional[int]


@dataclass
class Preset:
    num: int
    """Map rack space nums to fade times in seconds"""
    fade_time: dict[int, int] = dataclasses.field(default=None)
    """Map rack circuit nums to levels 0-255"""
    levels: dict[int, int] = dataclasses.field(default=None)

    def __post_init__(self):
        if self.fade_time is None:
            self.fade_time = {k: v for k, v in zip(range(1, 17), repeat(0, 16))}
        if self.levels is None:
            self.levels = {k: v for k, v in zip(range(1, 49), repeat(0, 48))}


def optional_int(val: Optional) -> Optional[int]:
    if isinstance(val, str) and len(val) == 0:
        return None
    try:
        return int(val)
    except ValueError:
        return None


class EchoRackConfigReader(sax.ContentHandler):
    circuits: dict[int, Circuit]
    rack_spaces: bidict[int, int]
    presets: dict[int, Preset]
    _current_preset: Optional[Preset] = None

    def __init__(self, circuits: dict[int, Circuit], rack_spaces: bidict[int, int], presets: dict[int, Preset]):
        super().__init__()
        self.circuits = circuits
        self.rack_spaces = rack_spaces
        self.presets = presets

    def _store_current_preset(self):
        if self._current_preset is not None:
            self.presets[self._current_preset.num] = self._current_preset
            self._current_preset = None

    def _process_relay(self, attrs):
        circuit = Circuit(
            num=int(attrs['NUMBER']),
            space_num=int(attrs['SPACE']),
            zone_num=int(attrs['ZONE'])
        )
        self.circuits[circuit.num] = circuit

    def _process_space(self, attrs):
        if 'SPACEINRACK' in attrs:
            echo_num = int(attrs['NUMBER'])
            if echo_num <= 0:
                return
            self.rack_spaces[int(attrs['SPACEINRACK'])] = echo_num
        elif 'SPACEINRACKEXT' in attrs:
            echo_num = int(attrs['NUMBEREXT'])
            if echo_num <= 0:
                return
            self.rack_spaces[int(attrs['SPACEINRACKEXT'])] = echo_num

    def _process_preset(self, attrs):
        self._store_current_preset()
        self._current_preset = Preset(num=int(attrs['NUMBER']))

    def _process_prefadelevel(self, attrs):
        self._current_preset.fade_time[int(attrs['SPACEINRACK'])] = int(attrs['UPTIME'])

    def _process_prelevel(self, attrs):
        self._current_preset.levels[int(attrs['RELAY'])] = int(attrs['LEVEL'])

    _element_processors = {
        'RELAY': _process_relay,
        'SPACE': _process_space,
        'PRESET': _process_preset,
        'PREFADELEVEL': _process_prefadelevel,
        'PRELEVEL': _process_prelevel,
    }

    def startElement(self, name, attrs):
        if name in self._element_processors:
            self._element_processors[name](self, attrs)
        else:
            super().startElement(name, attrs)

    def endDocument(self):
        self._store_current_preset()
        super().endDocument()


def parse_cfg(cfg_path: Path):
    circuits: dict[int, Circuit] = {}
    rack_spaces: bidict[int, int] = bidict()
    presets: dict[int, Preset] = {}

    # Get data from config file.
    parser = sax.make_parser()
    parser_handler = EchoRackConfigReader(circuits, rack_spaces, presets)
    parser.setContentHandler(parser_handler)
    parser.parse(cfg_path)

    return {
        'circuits': circuits,
        'rack_spaces': rack_spaces,
        'presets': presets
    }


class CfgMerger(XMLFilterBase):
    circuits: dict[int, Circuit]
    rack_spaces: bidict[int, int]
    presets: dict[int, Preset]
    _current_preset: Optional[Preset] = None

    def __init__(self, circuits: dict[int, Circuit], rack_spaces: bidict[int, int], presets: dict[int, Preset],
                 parent=None):
        super().__init__(parent)
        self.circuits = circuits
        self.rack_spaces = rack_spaces
        self.presets = presets

    def _process_relay(self, attrs):
        circuit = self.circuits.get(int(attrs['NUMBER']))
        if circuit is None:
            return
        if circuit.space_num is not None:
            attrs['SPACE'] = str(circuit.space_num)
        if circuit.zone_num is not None:
            attrs['ZONE'] = str(circuit.zone_num)

    def _process_space(self, attrs):
        if 'SPACEINRACK' in attrs:
            rack_space_num = int(attrs['SPACEINRACK'])
        elif 'SPACEINRACKEXT' in attrs:
            rack_space_num = int(attrs['SPACEINRACKEXT'])
        else:
            raise RuntimeError('Bad Rack Space element.')
        echo_space_num = self.rack_spaces.get(rack_space_num)
        if echo_space_num is None:
            return
        if echo_space_num > 16 or echo_space_num < 1:
            ext = 'EXT'
            non_ext = ''
        else:
            ext = ''
            non_ext = 'EXT'
        new_attrs = {
            'SPACEINRACK': str(rack_space_num),
            'NUMBER': str(echo_space_num),
            'NAME': ''
        }
        for k, v in new_attrs.items():
            attrs[f'{k}{ext}'] = v
            if f'{k}{non_ext}' in attrs:
                del attrs[f'{k}{non_ext}']

    def _process_preset(self, attrs):
        self._current_preset = self.presets.get(int(attrs['NUMBER']))

    def _process_prefadelevel(self, attrs):
        if self._current_preset is None:
            return
        rack_space_num = int(attrs['SPACEINRACK'])
        uptime = self._current_preset.fade_time.get(rack_space_num)
        if uptime is not None:
            attrs['UPTIME'] = str(uptime)

    def _process_prelevel(self, attrs):
        if self._current_preset is None:
            return
        relay_num = int(attrs['RELAY'])
        level = self._current_preset.levels.get(relay_num)
        if level is not None:
            attrs['LEVEL'] = str(level)

    _element_processors = {
        'RELAY': _process_relay,
        'SPACE': _process_space,
        'PRESET': _process_preset,
        'PREFADELEVEL': _process_prefadelevel,
        'PRELEVEL': _process_prelevel,
    }

    def startElement(self, name, attrs):
        attrs = dict(attrs)
        if name in self._element_processors:
            self._element_processors[name](self, attrs)
        super().startElement(name, AttributesImpl(attrs))


def config_to_ods(cfg_path: Path, ods_path: Path):
    cfg = parse_cfg(cfg_path)
    circuits = cfg['circuits']
    rack_spaces = cfg['rack_spaces']
    presets = cfg['presets']
    max_preset = max(presets.keys())

    # Write ODS.
    # Levels.
    levels_content = []
    # Header.
    levels_header = ['Circuit', 'Space', 'Zone']
    for preset_num in range(1, max_preset + 1):
        levels_header.append(f'Preset {preset_num}')
    levels_content.append(levels_header)
    # Values.
    for circuit in circuits.values():
        row = [
            circuit.num,
            circuit.space_num,
            circuit.zone_num
        ]
        for preset_num in range(1, max_preset + 1):
            row.append(presets[preset_num].levels[circuit.num])
        levels_content.append(row)

    # Times.
    times_content = []
    # Header.
    times_header = ['Space']
    for preset_num in range(1, max_preset + 1):
        times_header.append(f'Preset {preset_num}')
    times_content.append(times_header)
    # Values.
    for rack_space, echo_space in rack_spaces.items():
        row = [echo_space]
        for preset_num in range(1, max_preset + 1):
            row.append(presets[preset_num].fade_time[rack_space])
        times_content.append(row)

    # Write Book.
    book = pyexcel.get_book(bookdict={
        'Levels': levels_content,
        'Times': times_content,
    })
    book.save_as(ods_path)


def ods_to_config(cfg_path: Path, ods_path: Path, out_path: Path):
    cfg = parse_cfg(cfg_path)
    circuits = cfg['circuits']
    rack_spaces = cfg['rack_spaces']
    presets = cfg['presets']

    # Get data from ODS file.
    book = pyexcel.get_book(file_name=ods_path)
    re_preset = re.compile(r'^Preset (\d+)$')
    # Levels.
    levels_sheet: pyexcel.Sheet = book.sheet_by_name('Levels')
    levels_sheet.name_columns_by_row(0)
    levels_content = levels_sheet.to_records()
    for row in levels_content:
        circuit_num = optional_int(row['Circuit'])
        if circuit_num is None:
            continue
        space_num = optional_int(row['Space'])
        zone_num = optional_int(row['Zone'])
        circuit = Circuit(
            num=circuit_num,
            space_num=space_num,
            zone_num=zone_num
        )
        circuits[circuit.num] = circuit
        for col, val in row.items():
            if match := (re.match(re_preset, col)):
                preset_num = int(match.group(1))
                level = optional_int(val)
                if preset_num not in presets:
                    presets[preset_num] = Preset(num=preset_num)
                presets[preset_num].levels[circuit.num] = level

    # Times.
    times_sheet: pyexcel.Sheet = book.sheet_by_name('Times')
    times_sheet.name_columns_by_row(0)
    times_content = times_sheet.to_records()
    for row in times_content:
        space_num = optional_int(row['Space'])
        if space_num is None:
            continue
        for col, val in row.items():
            if match := (re.match(re_preset, col)):
                preset_num = int(match.group(1))
                uptime = optional_int(val)
                if preset_num not in presets:
                    presets[preset_num] = Preset(num=preset_num)
                # Preset fade times are indexed by rack space, not Echo space.
                presets[preset_num].fade_time[rack_spaces.inverse[space_num]] = uptime

    # Write config.
    reader = CfgMerger(circuits, rack_spaces, presets, sax.make_parser())
    with out_path.open('w') as out_file:
        parser_handler = XMLGenerator(out_file, encoding='utf-8', short_empty_elements=True)
        reader.setContentHandler(parser_handler)
        reader.parse(cfg_path)


def main():
    parser = argparse.ArgumentParser(description='Edit ETC Echo panel presets as a spreadsheet.')
    subparsers = parser.add_subparsers(title='subcommands')
    to_ods_parser = subparsers.add_parser('to_ods', help='Convert a panel config to ODS.')
    to_ods_parser.set_defaults(command='to_ods')
    to_ods_parser.add_argument('cfg', type=lambda p:Path(p), help='Path to saved panel config.')
    to_ods_parser.add_argument('ods', type=lambda p:Path(p), help='Path to generated ODS file.')
    to_cfg_parser = subparsers.add_parser('to_cfg', help='Convert an ODS file to a panel config.')
    to_cfg_parser.set_defaults(command='to_cfg')
    to_cfg_parser.add_argument('cfg', type=lambda p: Path(p), help='Path to original panel config.')
    to_cfg_parser.add_argument('ods', type=lambda p: Path(p), help='Path to generated ODS file.')
    to_cfg_parser.add_argument('out', type=lambda p: Path(p), help='Path to new panel config.')
    args = parser.parse_args()

    if args.command == 'to_ods':
        config_to_ods(args.cfg, args.ods)
    elif args.command == 'to_cfg':
        ods_to_config(args.cfg, args.ods, args.out)


if __name__ == '__main__':
    main()
