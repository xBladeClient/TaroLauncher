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

## Discord ("Jugando TaroLauncher")

TaroLauncher muestra en Discord **Jugando ahora: TaroLauncher** con su ícono (también al lado de tu nombre
cuando estás en una llamada), qué perfil estás jugando y hace cuánto tiempo, y un botón **Descargar TaroLauncher**.

Para activarlo hace falta una aplicación de Discord (gratis, se hace una sola vez):

1. Entrá a <https://discord.com/developers/applications> y tocá **New Application**. Nombre: **TaroLauncher**
   (es el texto que aparece en "Jugando ahora").
2. En **General Information** subí el ícono en **App Icon** (es el ícono que se ve en las llamadas) y guardá.
3. Copiá el **Application ID**.
4. En GitHub, editá [`TaroModpack/modpack.json`](TaroModpack/modpack.json) y pegalo en `"discord_client_id"`.
   Al guardar se genera un instalador nuevo con Discord activado.

Campos relacionados en `modpack.json`: `discord_image` (imagen grande de la actividad) y `download_url`
(link del botón).

## Cuentas premium (Microsoft)

Las cuentas **sin conexión** funcionan siempre. Para las **premium** Microsoft exige que cada launcher tenga
su propia aplicación registrada (la de Prism no se puede reutilizar):

1. Entrá a <https://portal.azure.com> → **Microsoft Entra ID** → **App registrations** → **New registration**.
   - Name: **TaroLauncher**
   - Supported account types: **Personal Microsoft accounts only**
   - Redirect URI: plataforma **Public client/native (mobile & desktop)** con `http://localhost`
2. En la app creada → **Authentication** → **Allow public client flows: Yes** → Save.
3. Copiá el **Application (client) ID** y pegalo en `"microsoft_client_id"` de
   [`TaroModpack/modpack.json`](TaroModpack/modpack.json).
4. Pedile a Mojang que apruebe la app en <https://aka.ms/mce-reviewappid> (puede tardar días o semanas).
   Hasta que la aprueben, el login premium da error "Invalid app registration".
