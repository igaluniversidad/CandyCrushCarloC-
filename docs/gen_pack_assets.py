# Script que pasa el pack "match-3-game-asset-ui-effects-free" (Cutie Tutti
# Frutti, de Ajay Karat) a la carpeta Assets del juego.
# Las gemas son los personajes del pack tal cual. Las bombas son el mismo
# personaje con franjas blancas y un brillo, hechos aqui con PIL.
import os, shutil
from PIL import Image, ImageDraw, ImageFont

PACK = r"C:\Users\igalp\AppData\Local\Temp\opencode\match3pack"
OUT = r"C:\Users\igalp\Documents\GitHub\CandyCrushCarloC++\SDLDrawReady\Assets"
os.makedirs(OUT, exist_ok=True)

# gem0..gem5 = 6 personajes con colores bien distintos
MAPA = {
    "gem0.png": r"characters\characters_0002.png",  # manzana roja
    "gem1.png": r"characters\characters_0003.png",  # naranja
    "gem2.png": r"characters\characters_0005.png",  # verde
    "gem3.png": r"characters\characters_0006.png",  # coco cafe
    "gem4.png": r"characters\characters_0007.png",  # gema rosa
    "gem5.png": r"characters\characters_0001.png",  # leche blanca
}
for dst, src in MAPA.items():
    shutil.copy(os.path.join(PACK, src), os.path.join(OUT, dst))

# mascota del menu (el pan tostado) y logo del pack
shutil.copy(os.path.join(PACK, r"characters\characters_0004.png"), os.path.join(OUT, "mascot.png"))
shutil.copy(os.path.join(PACK, r"ui_assets\game_title.png"), os.path.join(OUT, "logo.png"))

# fondo, boton, panel y fuente del pack
shutil.copy(os.path.join(PACK, r"ui_assets\bg.png"), os.path.join(OUT, "bg.png"))
shutil.copy(os.path.join(PACK, r"ui_assets\button_9slice01.png"), os.path.join(OUT, "btn.png"))
# el boton trae mucho transparente alrededor: lo recorto a lo amarillo
btn = Image.open(os.path.join(OUT, "btn.png")).convert("RGBA")
l, t, r, b = btn.split()[3].getbbox()
btn.crop((max(0, l - 4), max(0, t - 4), r + 4, b + 4)).save(os.path.join(OUT, "btn.png"))
shutil.copy(os.path.join(PACK, r"ui_assets\popup_base.png"), os.path.join(OUT, "panel.png"))
shutil.copy(os.path.join(PACK, r"font\SnowDream.TTF"), os.path.join(OUT, "font.ttf"))

# Bombas: mismo personaje + 3 franjas blancas diagonales + marco de luz.
S = 256
for i in range(6):
    base = Image.open(os.path.join(OUT, f"gem{i}.png")).convert("RGBA")
    if base.size != (S, S):
        base = base.resize((S, S))
    d = ImageDraw.Draw(base)
    cx = cy = S // 2
    for off in (-52, 0, 52):
        d.polygon([(cx + off - 22, 0), (cx + off + 6, 0),
                   (cx + off - 40, S), (cx + off - 68, S)],
                  fill=(255, 255, 255, 215))
    d.ellipse([4, 4, S - 4, S - 4], outline=(255, 255, 255, 230), width=6)
    d.ellipse([cx - 20, cy - 26, cx + 20, cy + 14], fill=(255, 255, 255, 255))
    base.save(os.path.join(OUT, f"gem{i}_bomb.png"))

# Pruebo que la fuente del pack se puede abrir y dibujar
f = ImageFont.truetype(os.path.join(OUT, "font.ttf"), 40)
print("fuente ok:", f.getname())
print("assets ok:", sorted(os.listdir(OUT)))
