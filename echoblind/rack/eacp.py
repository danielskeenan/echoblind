import xml.etree.ElementTree as ET
from xml import sax

from bidict import bidict
from packaging.version import Version, InvalidVersion

from echoblind.echotypes import Circuit, Preset
from echoblind.rack.configreader import BaseConfigReader
from echoblind.rack.configwriter import BaseConfigWriter
from echoblind.rack.detector import BaseDetector


class EchoACPConfigReader(BaseConfigReader):
    OUTPUT_TAG_NAME = 'OUTPUT'
    OUTPUT_ATTR_NAME = 'OUTPUT'
    FADETIME_ATTR_NAME = 'PREFADELEVEL'


class EchoACPConfigWriter(BaseConfigWriter):
    OUTPUT_TAG_NAME = 'OUTPUT'
    OUTPUT_ATTR_NAME = 'OUTPUT'
    FADETIME_ATTR_NAME = 'PREFADELEVEL'


class EchoACPDetector(BaseDetector):
    def is_valid(self) -> bool:
        tree = ET.parse(self.path)
        root = tree.getroot()

        # Is this a PCP.
        if root.tag != 'EACP':
            return False

        # Check version.
        rack = root.find("RACK")
        if rack is None:
            return False
        rack_version = rack.attrib.get('VERSION')
        if rack_version is None:
            return False
        try:
            rack_version = Version(rack_version)
        except InvalidVersion:
            return False
        if rack_version < Version("1.3") or rack_version >= Version("2.1"):
            return False

        return True

    def get_reader(self, circuits: dict[int, Circuit], rack_spaces: bidict[int, int],
                   presets: dict[int, Preset]):
        parser = sax.make_parser()
        parser_handler = EchoACPConfigReader(circuits, rack_spaces, presets)
        parser.setContentHandler(parser_handler)

        return parser

    def get_writer(self, circuits: dict[int, Circuit], rack_spaces: bidict[int, int], presets: dict[int, Preset],
                   parent=None):
        return EchoACPConfigWriter(circuits, rack_spaces, presets, sax.make_parser())
