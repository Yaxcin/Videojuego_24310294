# Tower Defense Mexicano 🎮

Tower Defense inspirado en Bloons, pero con una temática mexicana completa.
Basado en la estructura de proyecto de **EjemplosJuego** del profesor.

## Tema del Juego
- **Enemigos**: Piñatas (Engrudo, Arcilla, Revelación, Fruta, Hipnotizadora)
- **Torres**: Personajes mexicanos (Taquero, Abuelita, Don de Cohetes, Organillero, Raspadero)
- **Monedas**: Dulces de chocolate de la piñata "Bolo Bolo"

## Requisitos
- **Compilador**: MinGW64 (via MSYS2)
- **SFML 3.x**: Instalado en MSYS2
- **make**: Parte de mingw64-toolchain
- **PowerShell 5+** o bash (para scripts de compilación)

## Compilación

### Opción 1: Script PowerShell (Recomendado en Windows)
```powershell
.\compile.ps1
```

### Opción 2: Makefile (Recomendado en cualquier terminal)
```bash
make run
```

### Opción 3: Compilación manual
```bash
mkdir -Force obj, bin
g++ -std=c++17 -Iinclude -c src/main.cpp -o obj/main.o
g++ -std=c++17 -Iinclude -c src/Entity.cpp -o obj/Entity.o
g++ -std=c++17 obj/main.o obj/Entity.o -o bin/TowerDefenseMexicano.exe -lsfml-graphics -lsfml-window -lsfml-system
```

## Ejecución
```powershell
.\bin\TowerDefenseMexicano.exe
```

## Controles (Fase 1 - Debug)
- **ESC**: Cerrar juego
- **1-6**: Cambiar estados de prueba
  - **1**: MENU (azul)
  - **2**: ROUND_ACTIVE (verde)
  - **3**: ROUND_PAUSE (amarillo)
  - **4**: COLLECTING_COINS (magenta)
  - **5**: VICTORY (cyan)
  - **6**: DEFEAT (rojo)
- **M**: +100 dinero (test)
- **L**: -1 vida (test)

## Estructura del Proyecto

```
.
├── include/
│   ├── GameWindow.hpp     # Wrapper para sf::RenderWindow
│   ├── GameState.h        # Enum de estados
│   ├── Entity.h           # Clase base de entidades
│   ├── Enemies/           # (Fase 2) Piñatas
│   └── Towers/            # (Fase 3) Torres/Personajes
├── src/
│   ├── main.cpp           # Punto de entrada, bucle principal
│   ├── Entity.cpp         # Implementación de Entity
│   ├── Enemies/           # (Fase 2) Implementaciones
│   └── Towers/            # (Fase 3) Implementaciones
├── assets/
│   ├── textures/          # Sprites (Fase 5)
│   ├── sounds/            # Audio (Fase 5)
│   └── fonts/             # Tipografías (Fase 5)
├── bin/                   # Ejecutable compilado
├── obj/                   # Archivos objeto (compilación)
├── makefile               # Sistema de compilación
├── compile.ps1            # Script PowerShell de compilación
└── README.md              # Este archivo
```

## Comandos make útiles

```bash
make all       # Compilar proyecto
make run       # Compilar y ejecutar
make clean     # Limpiar compilación
make rebuild   # Limpiar y recompilar
make help      # Mostrar ayuda
```

## Estado del Proyecto

### ✓ Fase 1: Infraestructura Base (COMPLETADA)
- ✓ Configuración SFML 3.x
- ✓ Bucle principal a 60 FPS
- ✓ Máquina de estados con 6 estados
- ✓ GameWindow wrapper
- ✓ Estructura compatible con EjemplosJuego

### ⏳ Fase 2: Sistema de Piñatas (PRÓXIMO)
- [ ] Clase Enemy base
- [ ] 5 tipos de piñatas específicas
- [ ] Sistema Path/Carril
- [ ] EnemySpawner

### ⏳ Fase 3: Sistema de Torres
- [ ] Clase Tower base
- [ ] 5 torres/personajes
- [ ] Sistema de targeting
- [ ] Sistema de proyectiles

### ⏳ Fase 4: Flujo de Olas y Economía
- [ ] RoundManager
- [ ] Sistema de dinero
- [ ] Compra/venta entre rondas
- [ ] Mecánica "Bolo Bolo"

### ⏳ Fase 5: UI y Pulido
- [ ] HUD en pantalla
- [ ] Gráficos y animaciones
- [ ] Sonidos y música
- [ ] Menús

## Notas Técnicas
- **C++**: Estándar C++17
- **SFML 3.x**: Adaptado a nueva API (std::optional para eventos)
- **Compilador**: MinGW64 g++ 16.1.0
- **Estructura**: Basada en EjemplosJuego del repositorio del profesor

## Recursos Útiles
- [Repositorio de ejemplos](C:\Users\higar\OneDrive\Escritorio\EjemplosJuego)
- [Documentación SFML 3](https://www.sfml-dev.org/documentation/3.0/)
- [MinGW64 via MSYS2](https://www.msys2.org/)

---

**Versión**: 0.1.1 (Fase 1 + Estructura EjemplosJuego)
**Última actualización**: 2026-06-04
