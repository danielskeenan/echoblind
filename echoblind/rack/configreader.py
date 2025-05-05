from abc import ABC, abstractmethod
from typing import Optional
from xml import sax

from bidict import bidict

from echoblind.echotypes import Circuit, Preset


class BaseConfigReader(ABC, sax.ContentHandler):
    """Base class for config readers."""
    OUTPUT_TAG_NAME = 'OUTPUT'
    OUTPUT_ATTR_NAME = 'OUTPUT'

    circuits: dict[int, Circuit]
    rack_spaces: bidict[int, int]
    presets: dict[int, Preset]
    current_preset: Optional[Preset] = None

    def __init__(self, circuits: dict[int, Circuit], rack_spaces: bidict[int, int], presets: dict[int, Preset]):
        super().__init__()
        self.circuits = circuits
        self.rack_spaces = rack_spaces
        self.presets = presets

    def store_current_preset(self):
        if self.current_preset is not None:
            self.presets[self.current_preset.num] = self.current_preset
            self.current_preset = None

    def process_output(self, attrs):
        circuit = Circuit(
            num=int(attrs['NUMBER']),
            space_num=int(attrs['SPACE']),
            zone_num=int(attrs['ZONE'])
        )
        self.circuits[circuit.num] = circuit

    def process_space(self, attrs):
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

    def process_preset(self, attrs):
        self.store_current_preset()
        self.current_preset = Preset(num=int(attrs['NUMBER']))

    def process_prefadelevel(self, attrs):
        self.current_preset.fade_time[int(attrs['SPACEINRACK'])] = int(attrs['UPTIME'])

    def process_prelevel(self, attrs):
        self.current_preset.levels[int(attrs[self.OUTPUT_ATTR_NAME])] = int(attrs['LEVEL'])

    def startElement(self, name, attrs):
        element_processors = {
            self.OUTPUT_TAG_NAME: self.process_output,
            'SPACE': self.process_space,
            'PRESET': self.process_preset,
            'PREFADELEVEL': self.process_prefadelevel,
            'PRELEVEL': self.process_prelevel,
        }

        if name in element_processors:
            element_processors[name](attrs)
        else:
            super().startElement(name, attrs)

    def endDocument(self):
        self.store_current_preset()
        super().endDocument()
