from pathlib import Path
root=Path(__file__).resolve().parent.parent
editor=(root/"Source/PluginEditor.cpp").read_text(encoding="utf-8")
h=(root/"Source/PedalDSP.h").read_text(encoding="utf-8")
assert "EXPLORER LES 80 FICHES" in editor
assert '"80 ORIGINAL PEDALS"' in editor
assert 'historyModel<=1?pedal::kModelCount-1:historyModel-1' in editor
assert 'historyModel>=pedal::kModelCount-1?1:historyModel+1' in editor
assert "80 MODÈLES DSP" in editor
assert "Model::Count" not in h or "constexpr int kModelCount" in h
assert "setSize(1600,930)" in editor
assert "r.getWidth()-12" in editor
print("PASS: encyclopedia, browsing, and fixed-canvas references updated to 80")