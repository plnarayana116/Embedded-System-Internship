# NPM1100 Power Management IC Reference Board

## Design Overview
This reference board implements NordicSemi's NPM1100 Power Management IC in a compact 35mm x 35mm form factor.

## Key Features
- Input: 8V via 2-pin header
- Output: 3.0V regulated
- Charge Current: 200mA constant current
- Battery voltage monitoring circuit
- Power and charging status LEDs
- MCU interface headers

## Design Specifications
- PCB Size: 35mm × 35mm
- Layers: 4-layer stackup
- Component Size: 0402 to 0805
- All SMD components top-mounted

## Files Included
- `Schematic.pdf`: Complete circuit schematic
- `pcb_design.pdf`: PCB layout and design details
- `BOM.csv`: Bill of Materials with LCSC part numbers

## Design Notes
- Proper decoupling implemented near IC
- Ground and power planes for better EMI performance
- Clear silkscreen labeling for easy assembly
- Test points included for debugging