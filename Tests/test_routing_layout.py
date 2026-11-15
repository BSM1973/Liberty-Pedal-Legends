"""Static geometry invariants for the approved 1600x930 JUCE layout.
These are regression checks, not a substitute for a real DAW visual test.
"""
from pathlib import Path
root=Path(__file__).resolve().parents[1]
ui=(root/'Source/PluginEditor.cpp').read_text(encoding='utf-8')
hdr=(root/'Source/PluginEditor.h').read_text(encoding='utf-8')
processor=(root/'Source/PluginProcessor.cpp').read_text(encoding='utf-8')
engine=(root/'Source/PedalDSP.cpp').read_text(encoding='utf-8')
presets=(root/'Source/FactoryPresets.h').read_text(encoding='utf-8')
assert 'canvasWidth=1600.f,canvasHeight=930.f' in hdr
assert 'const float left=56.f,w=355.f,space=26.f;' in ui
assert '177.f+row*294.f,w,281.f' in ui
assert 'drawKnob(g,174,824,20' in ui and 'drawKnob(g,1280,824,19' in ui
assert 'Screen::Routing' in ui and 'drawRouting(juce::Graphics& g)' in ui
assert 'kOriginalSeriesPresetCount = 100' in presets and 'kOriginalParallelPresetCount = 100' in presets
assert 'kAddedSeriesPresetCount = 100' in presets and 'kAddedParallelPresetCount = 100' in presets
assert 'kFactoryPresetCount = kSeriesPresetCount + kParallelPresetCount' in presets
assert 'routing_parallel' in processor and 'routing_blend' in processor and '_branch' in processor
assert 'lane[0][ch]=wet[ch]' in engine and 'lane[1][ch]=wet[ch]' in engine
assert 'float& signal=parallel?lane[branch][ch]:wet[ch]' in engine
# Rectangles in the new overlay must be in the same 1600 x 930 canvas.
windows = [(123,93,1354,753),(321,120,958,685)]
for x,y,w,h in windows:
    assert x>=0 and y>=0 and x+w<=1600 and y+h<=930

def overlaps(a,b):
    x,y,w,h=a;xx,yy,ww,hh=b
    return x<xx+ww and xx<x+w and y<yy+hh and yy<y+h
# New browser cards occupy three columns and three rows without intersection.
cards = [(382+col*282,400+row*98,271,89) for row in range(3) for col in range(3)]
assert all(not overlaps(a,b) for i,a in enumerate(cards) for b in cards[i+1:])
assert all(x>377 and y>390 and x+w<1279 and y+h<703 for x,y,w,h in cards)
# Routing sections have an explicit vertical gap, slider entirely below them.
lanes=[(174,326,1224,166),(174,513,1224,166)]
slider=(174,724,1222,86)
assert not overlaps(*lanes)
assert all(not overlaps(r,slider) for r in lanes)
# Up to 8 nodes fit inside the lane's interior without touching their neighbors.
for n in range(1,9):
    step=1064./n
    w=min(136.,step-12.)
    boxes=[(300+(i+.5)*step-w/2,0,w,121) for i in range(n)]
    assert all(x>=296 and x+w<=1364 for x,y,w,h in boxes)
    assert all(not overlaps(a,b) for i,a in enumerate(boxes) for b in boxes[i+1:])
print('Routing layout checks OK: immutable canvas/board, 8-node lanes and non-overlapping cards')
