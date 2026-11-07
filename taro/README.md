# TaroModpack: cómo poner tus mods y configs

Esta carpeta es la **plantilla** del modpack que viene con TaroLauncher.
El instalador la copia junto al `.exe` y, cada vez que se abre el launcher,
la instancia **TaroModpack** se crea (si no existe) o se actualiza (si subiste la versión).

```
taro/TaroModpack/
├── instance.cfg          ← nombre e ícono de la instancia
├── mmc-pack.json         ← versión de Minecraft (26.3) y de Fabric Loader (0.19.5)
├── taro_modpack.json     ← número de versión del modpack  ← SUBILO cuando cambies algo
├── icon.png              ← ícono del modpack
└── minecraft/
    ├── mods/             ← acá van los .jar (los de KeoOptimized)
    ├── config/           ← acá va la carpeta config de KeoOptimized
    ├── resourcepacks/    ← opcional
    └── shaderpacks/      ← opcional
```

## Pasos para meter KeoOptimized

1. Abrí KeoOptimized en el launcher donde lo tengas y entrá a su carpeta `.minecraft` (o `minecraft`).
2. Copiá **todo el contenido** de su carpeta `mods` a `taro/TaroModpack/minecraft/mods/`.
3. Copiá **todo el contenido** de su carpeta `config` a `taro/TaroModpack/minecraft/config/`.
4. Si trae `resourcepacks`, `shaderpacks`, `options.txt`, etc., copialos también dentro de `minecraft/`.
5. Si KeoOptimized usa otra versión de Fabric Loader, cambiala en `mmc-pack.json`.
6. Abrí `taro_modpack.json` y **subí el número** (`1` → `2` → `3`...). Así los jugadores que ya tienen
   el launcher reciben la actualización.
7. Compilá de nuevo (o hacé push a GitHub y esperá el instalador nuevo).

## Qué pasa cuando actualizás el modpack

Si el número de `taro_modpack.json` es mayor que el que tiene el jugador:

- La carpeta `mods` se **reemplaza entera** (así no quedan mods viejos).
- `config`, `resourcepacks`, `shaderpacks` y demás se **sobrescriben** archivo por archivo.
- `mmc-pack.json` se actualiza (versión de Minecraft/Fabric).
- Se **conservan**: mundos (`saves`), capturas, `options.txt`, `servers.dat` y la configuración de la instancia (RAM, Java, tiempo jugado).

Si un jugador borra la instancia, se vuelve a crear la próxima vez que abra el launcher.
