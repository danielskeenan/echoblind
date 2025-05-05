import xml.etree.ElementTree as ET
from xml import sax

from bidict import bidict
from packaging.version import Version, InvalidVersion

from echoblind.echotypes import Circuit, Preset
from echoblind.rack.configreader import BaseConfigReader
from echoblind.rack.configwriter import BaseConfigWriter
from echoblind.rack.detector import BaseDetector


class EchoPCPConfigReader(BaseConfigReader):
    OUTPUT_TAG_NAME = 'RELAY'
    OUTPUT_ATTR_NAME = 'RELAY'


class EchoPCPConfigWriter(BaseConfigWriter):
    OUTPUT_TAG_NAME = 'RELAY'
    OUTPUT_ATTR_NAME = 'RELAY'

class EchoPCPDetector(BaseDetector):
    def is_valid(self) -> bool:
        tree = ET.parse(self.path)
        root = tree.getroot()

        # Is this a PCP.
        if root.tag != 'SMARTSWITCH2':
            return False

        # Check version.
        cabinet = root.find("CABINET")
        if cabinet is None:
            return False
        cabinet_version = cabinet.attrib.get('VERSION')
        if cabinet_version is None:
            return False
        try:
            cabinet_version = Version(cabinet_version)
        except InvalidVersion:
            return False
        if cabinet_version < Version("3.1") or cabinet_version >= Version("4"):
            return False

        return True

    def get_reader(self, circuits: dict[int, Circuit], rack_spaces: bidict[int, int],
                   presets: dict[int, Preset]):
        parser = sax.make_parser()
        parser_handler = EchoPCPConfigReader(circuits, rack_spaces, presets)
        parser.setContentHandler(parser_handler)

        return parser

    def get_writer(self, circuits: dict[int, Circuit], rack_spaces: bidict[int, int], presets: dict[int, Preset],
                   parent=None):
        return EchoPCPConfigWriter(circuits, rack_spaces, presets, sax.make_parser())
