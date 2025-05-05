from abc import ABC, abstractmethod
from pathlib import Path

from bidict import bidict

from echoblind.echotypes import Circuit, Preset
from echoblind.rack.configreader import BaseConfigReader


class BaseDetector(ABC):
    """Base class for detectors, i.e. classes that detect if the config file is of a certain type."""
    path: Path

    def __init__(self, path: Path):
        self.path = path

    @abstractmethod
    def is_valid(self) -> bool:
        """Is this path of the type this class detects?"""
        pass

    @abstractmethod
    def get_reader(self, circuits: dict[int, Circuit], rack_spaces: bidict[int, int], presets: dict[int, Preset]):
        """Get the reader for the rack this class detects."""
        pass

    @abstractmethod
    def get_writer(self, circuits: dict[int, Circuit], rack_spaces: bidict[int, int], presets: dict[int, Preset],
                   parent=None):
        """Get the writer for the rack this class detects."""
        pass
