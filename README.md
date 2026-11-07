# TaroLauncher

Launcher de Minecraft
con **TaroModpack** (Minecraft 26.3 + Fabric) preinstalado.

- Al instalarlo con `TaroLauncherInstaller.exe` se crea un acceso directo en el escritorio y en el menú Inicio.
- Todos los datos (instancias, cuentas, configuración) se guardan en `%APPDATA%\TaroLauncher`.
- La primera vez que se abre, aparece la instancia **TaroModpack** lista para jugar. Si actualizás el
  modpack, los jugadores reciben los cambios solos al abrir el launcher (ver [`taro/README.md`](taro/README.md)).
- Si el jugador ya usaba Prism Launcher, TaroLauncher ofrece importar sus instancias y cuentas.

## 1. Poner tus mods (KeoOptimized)

Copiá los mods y configs en `taro/TaroModpack/minecraft/` y subí el número de `taro/TaroModpack/taro_modpack.json`.
Instrucciones detalladas en [`taro/README.md`](taro/README.md).

## 2. Conseguir el instalador (GitHub Actions)

No hace falta instalar Visual Studio ni Qt: GitHub lo compila gratis.

1. Creá un repositorio **vacío** en GitHub llamado `TaroLauncher` (sin README ni licencia).
2. Instalá [GitHub Desktop](https://desktop.github.com/), elegí **File → Add local repository** y seleccioná esta carpeta.
3. Hacé click en **Publish repository** (podés dejarlo privado).
4. En GitHub, entrá a la pestaña **Actions**: el build arranca solo (tarda unos 30-60 minutos la primera vez).
5. Cuando termine, abrí el build y descargá el artefacto **TaroLauncher-Windows-MSVC-Setup-...**:
   adentro está `TaroLauncherInstaller.exe`.

Cada vez que hagas cambios (por ejemplo, mods nuevos), hacé **Commit** + **Push** en GitHub Desktop y se genera un instalador nuevo.

**Publicar una versión:** creá un tag que empiece con `v` (por ejemplo `v1.0.0`) y GitHub crea una Release
con `TaroLauncherInstaller.exe` adjunto para compartir.

## 3. Login con cuentas de Microsoft (premium)

Prism Launcher pide expresamente que los proyectos derivados **no** usen sus claves, así que TaroLauncher viene sin ellas:

| Función | Qué hacer |
|---|---|
| Cuentas Microsoft | Registrá una app en [Azure (Microsoft Entra)](https://learn.microsoft.com/es-es/entra/identity-platform/quickstart-register-app) para cuentas personales de Microsoft, y pedí acceso a la API de Minecraft con [este formulario de Mojang](https://aka.ms/mce-reviewappid). Después guardá el *Application (client) ID* como secreto `TARO_MSA_CLIENT_ID` en GitHub (**Settings → Secrets and variables → Actions**). |
| CurseForge | Pedí una clave en [console.curseforge.com](https://console.curseforge.com/) y guardala como secreto `TARO_CURSEFORGE_API_KEY`. |

Mientras tanto se puede jugar con **cuentas offline**, y Modrinth funciona sin clave.
También se puede poner un Client ID a mano en el launcher: **Configuración → APIs**.

## Licencia

TaroLauncher es software libre bajo **GPL-3.0** (ver [LICENSE](LICENSE) y [COPYING.md](COPYING.md)), igual que Prism Launcher,
en el que está basado. Si compartís el instalador, tenés que publicar también el código fuente (por ejemplo, con el repo público).
