import argparse
import csv
import dataclasses
from itertools import zip_longest, repeat
from pathlib import Path
from dataclasses import dataclass
from typing import Optional
from xml import sax

import pyexcel


@dataclass
class Circuit:
    num: int
    space_num: int
    zone_num: int


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


class EchoRackConfigReader(sax.ContentHandler):
    circuits: list[Circuit]
    rack_spaces: dict[int, int]
    presets: dict[int, Preset]
    _current_preset: Optional[Preset] = None

    def __init__(self, circuits: list[Circuit], rack_spaces: dict[int, int], presets: dict[int, Preset]):
        super().__init__()
        self.circuits = circuits
        self.rack_spaces = rack_spaces
        self.presets = presets

    def _store_current_preset(self):
        if self._current_preset is not None:
            self.presets[self._current_preset.num] = self._current_preset
            self._current_preset = None

    def _process_circuit(self, attrs):
        circuit = Circuit(
            num=int(attrs['NUMBER']),
            space_num=int(attrs['SPACE']),
            zone_num=int(attrs['ZONE'])
        )
        self.circuits.append(circuit)

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
        'RELAY': _process_circuit,
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


def config_to_ods(cfg_path: Path, ods_path: Path):
    circuits: list[Circuit] = []
    rack_spaces: dict[int, int] = {}
    presets: dict[int, Preset] = {}

    # Get data from config file.
    parser = sax.make_parser()
    parser_handler = EchoRackConfigReader(circuits, rack_spaces, presets)
    parser.setContentHandler(parser_handler)
    parser.parse(cfg_path)
    max_circuit = max(map(lambda o: o.num, circuits))
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
    for circuit in circuits:
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


def main():
    parser = argparse.ArgumentParser(description='Edit ETC Echo panel presets as a spreadsheet')
    parser.add_argument('command', choices=(
        'import', 'export'), help='Import a panel config to ODS, or Export a ODS into a panel config')
    parser.add_argument('--cfg', type=lambda xml_path: Path(xml_path), required=True, help='Path to panel config XML')
    parser.add_argument('--ods', type=lambda out_path: Path(out_path), required=True, help='Path to exported ODS')
    args = parser.parse_args()

    if args.command == 'import':
        config_to_ods(args.cfg, args.ods)


if __name__ == '__main__':
    main()
