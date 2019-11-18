  # Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [1.1.0-rc.5] - 2018-01-24 (from DUO SN 1168)
### Changed
- Call midi_handle and sequencer_update only twice per loop
- Clear held notes on power up
- Fixed: MIDI note on/offs via the physical port were not handled

## [1.1.0-rc.4] - 2018-01-15 
### Changed
- Fixed bug where MIDI channel was incorrectly stored in EEPROM

## [1.1.0-rc.3] - 2018-01-10 
### Changed
- Fixed bug where tempo pot didn't do anything when syncing to MIDI/sync

## [1.1.0-rc.2] - 2018-01-09 
### Added
- MIDI channel is stored even when DUO is turned off
- Sysex functions: reboot to bootloader and firmware version

## [1.1.0-rc.1] - 2017-12-14 
### Added
- MIDI channel is shown on the keyboard on startup
- MIDI channel is selectable through keyboard keys on startup
- Filtering of midi CC values
- Slider LEDs fade in on startup
- Sidechaining on the kick
- Sequencer stops before entering bootloader (only in DEV_MODE)

### Changed
- Releasing the random button brings you back to the original sequence
- No more clipping when filter and the two oscillators are set to approximately the same frequency and resonance is up