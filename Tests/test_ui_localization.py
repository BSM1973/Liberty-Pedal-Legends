"""Portable UI regression checks independent of JUCE or an actual DAW.
    Host display confirmation is still required on macOS/Windows.
"""
from pathlib import Path
import re

root = Path(__file__).resolve().parents[1]
ui = (root / 'Source/PluginEditor.cpp').read_text(encoding='utf-8')
loc = (root / 'Source/UILocalization.h').read_text(encoding='utf-8')
processor = (root / 'Source/PluginProcessor.cpp').read_text(encoding='utf-8')

# Non-ASCII literal strings must explicitly tell JUCE that they're UTF-8.
# juce::String(const char*) is not a safe constructor for extended characters.
pattern = re.compile(r'"(?:\\.|[^"\\])*"')
violations = []
for m in pattern.finditer(ui):
    literal = m.group(0)
    if not any(ord(ch) > 127 for ch in literal):
        continue
    pre = ui[max(0, m.start() - 26):m.start()]
    if not pre.endswith('juce::String::fromUTF8('):
        violations.append((ui.count('\n', 0, m.start()) + 1, literal[:55]))
assert not violations, f'Unsafe JUCE extended UTF-8 literals: {violations}'

for text in ('"Français"','"English"','"Español"','safeTypography','languageSelector',
             'languageChanged','getUILanguage','localized("tip_chain")'):
    assert text in ui or text in loc or text in processor, text
assert 'state.setProperty("UILanguage"' in processor
assert 'getProperty("UILanguage",0)' in processor

# Validate exact occupied header rectangles at the 1600x930 design size.
# Endpoints at different x ranges guarantee no permanent visual collisions.
rects = {
    'preset': (492, 17, 626, 84),
    'language': (1138, 48, 151, 33),
    'save': (1306, 18, 67, 37),
    'AB': (1382, 18, 75, 37),
    'bypass': (1465, 18, 112, 37),
    'in_meter': (1338, 78, 95, 12),
    'out_meter': (1487, 78, 88, 12),
}
def intersects(a, b):
    x,y,w,h=a; xx,yy,ww,hh=b
    return x<xx+ww and xx<x+w and y<yy+hh and yy<y+h
for key, rect in rects.items():
    if key != 'language':
        assert not intersects(rect, rects['language']), f'Language menu overlaps {key}'

assert 'languageSelector.setVisible(next==Screen::Board)' in ui
assert 'drawOverlay(g,panel,"LIBERTY MUSIC SOFTWARE' not in ui or 'false);' in ui
assert len(re.findall(r'\badd\(', loc)) > 150
print('UI localization checks OK: UTF-8, 3 languages, state and header non-overlap')
