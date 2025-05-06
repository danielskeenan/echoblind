import argparse
import re
from pathlib import Path
from xml import sax
from xml.sax.saxutils import XMLGenerator

import pyexcel
from bidict import bidict

from echoblind.echotypes import Circuit, optional_int, Preset
from echoblind.rack.eacp import EchoACPDetector
from echoblind.rack.epcp import EchoPCPDetector

DETECTOR_CLASSES = [
    EchoPCPDetector,
    EchoACPDetector
]


def parse_cfg(cfg_path: Path):
    circuits: dict[int, Circuit] = {}
    rack_spaces: bidict[int, int] = bidict()
    presets: dict[int, Preset] = {}

    # Get data from config file.
    parser = None
    for detector_class in DETECTOR_CLASSES:
        detector = detector_class(cfg_path)
        if detector.is_valid():
            parser = detector.get_reader(circuits, rack_spaces, presets)
            break
    if parser is None:
        raise RuntimeError("Could not find reader for this config file.")
    parser.parse(cfg_path)

    return {
        'circuits': circuits,
        'rack_spaces': rack_spaces,
        'presets': presets
    }


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
    parser = None
    for detector_class in DETECTOR_CLASSES:
        detector = detector_class(cfg_path)
        if detector.is_valid():
            parser = detector.get_writer(circuits, rack_spaces, presets, sax.make_parser())
            break
    if parser is None:
        raise RuntimeError("Could not find reader for this config file.")
    with out_path.open('w') as out_file:
        parser_handler = XMLGenerator(out_file, encoding='utf-8', short_empty_elements=True)
        parser.setContentHandler(parser_handler)
        parser.parse(cfg_path)


def main():
    parser = argparse.ArgumentParser(description='Edit ETC Echo panel presets as a spreadsheet.')
    subparsers = parser.add_subparsers(title='subcommands')
    to_ods_parser = subparsers.add_parser('to_ods', help='Convert a panel config to ODS.')
    to_ods_parser.set_defaults(command='to_ods')
    to_ods_parser.add_argument('cfg', type=lambda p: Path(p), help='Path to saved panel config.')
    to_ods_parser.add_argument('ods', type=lambda p: Path(p), help='Path to generated ODS file.')
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
