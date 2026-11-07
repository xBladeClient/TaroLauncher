# TaroModpack y menú principal

## Cómo funciona TaroModpack

TaroModpack **no guarda los mods en este repositorio**: se descarga de **Modrinth** la primera vez que se abre
TaroLauncher (con todos sus mods y configs) y se instala como el perfil **TaroModpack** con el ícono de Taro.

Cuando el modpack de Modrinth saca una versión nueva, el launcher pregunta al abrirse si querés actualizar.
Los mundos y los ajustes del jugador se conservan.

La configuración está en [`TaroModpack/modpack.json`](TaroModpack/modpack.json):

```json
{
    "name": "TaroModpack",
    "modrinth_project": "oePruTVg",
    "game_version": "26.3",
    "loader": "fabric",
    "news_url": "https://raw.githubusercontent.com/xBladeClient/TaroLauncher/main/taro/news.json"
}
```

| Campo | Qué es |
|---|---|
| `name` | Nombre del perfil en el launcher. |
| `modrinth_project` | ID (o slug) del modpack en Modrinth. `oePruTVg` = **Keo Optimized**. |
| `game_version` | Versión de Minecraft a usar. Si KeoOptimized saca una versión para otra versión de Minecraft, cambiala acá. |
| `loader` | `fabric`, `quilt`, `forge` o `neoforge`. |
| `news_url` | De dónde se leen las noticias del menú principal. |

`TaroModpack/icon.png` es el ícono del perfil.

## Noticias del menú principal

Las tarjetas de **Últimas noticias** se leen de [`news.json`](news.json) directamente desde GitHub,
así que **podés cambiarlas editando ese archivo en la web de GitHub**, sin sacar un instalador nuevo:

```json
[
    { "tag": "Lanzador", "title": "TaroLauncher ya está disponible", "date": "Noviembre 2026",
      "url": "https://github.com/xBladeClient/TaroLauncher/releases/latest" }
]
```

`url` es opcional: si la ponés, la tarjeta se abre en el navegador al hacer click.
