from abc import ABC, abstractmethod
from typing import Optional
from xml.sax.saxutils import XMLFilterBase
from xml.sax.xmlreader import AttributesImpl

from bidict import bidict

from echoblind.echotypes import Circuit, Preset


class BaseConfigWriter(ABC, XMLFilterBase):
    """Base class for config writers."""
    OUTPUT_TAG_NAME = 'OUTPUT'
    OUTPUT_ATTR_NAME = 'OUTPUT'

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

    def process_output(self, attrs):
        circuit = self.circuits.get(int(attrs['NUMBER']))
        if circuit is None:
            return
        if circuit.space_num is not None:
            attrs['SPACE'] = str(circuit.space_num)
        if circuit.zone_num is not None:
            attrs['ZONE'] = str(circuit.zone_num)

    def process_space(self, attrs):
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

    def process_preset(self, attrs):
        self._current_preset = self.presets.get(int(attrs['NUMBER']))

    def process_prefadelevel(self, attrs):
        if self._current_preset is None:
            return
        rack_space_num = int(attrs['SPACEINRACK'])
        uptime = self._current_preset.fade_time.get(rack_space_num)
        if uptime is not None:
            attrs['UPTIME'] = str(uptime)

    def process_prelevel(self, attrs):
        if self._current_preset is None:
            return
        relay_num = int(attrs[self.OUTPUT_ATTR_NAME])
        level = self._current_preset.levels.get(relay_num)
        if level is not None:
            attrs['LEVEL'] = str(level)

    def startElement(self, name, attrs):
        element_processors = {
            self.OUTPUT_TAG_NAME: self.process_output,
            'SPACE': self.process_space,
            'PRESET': self.process_preset,
            'PREFADELEVEL': self.process_prefadelevel,
            'PRELEVEL': self.process_prelevel,
        }

        attrs = dict(attrs)
        if name in element_processors:
            element_processors[name](attrs)
        super().startElement(name, AttributesImpl(attrs))
