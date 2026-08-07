# CalculadoraMonotributo

Calculadora de monotributo argentino con los valores vigentes desde febrero 2026 (fuente: ARCA, actualización IPC semestre 2/2025).

Hay dos versiones que comparten exactamente la misma tabla y la misma lógica de cálculo.

## Versión C (consola)

El programa original. Se puede ejecutar sin muchas complicaciones tan solo con su `.exe`.

```
gcc -o monotributo monotributo.c -lm
./monotributo
```

## Versión web

`docs/index.html` — un único archivo HTML, sin dependencias ni build. Se abre con doble clic o se publica en cualquier hosting estático.

Para publicarlo en GitHub Pages: **Settings → Pages → Source: rama `main`, carpeta `/docs`**.

Cubre las mismas cuatro funciones del menú de consola:

- **Mi categoría** — cálculo en vivo, con selector mensual/anual y servicios/venta de bienes. Muestra la cuota total, el desglose (impuesto integrado + SIPA + obra social), un medidor de uso del tope y la escala A→K.
- **Recategorización** — carga de los últimos 12 meses y comparación contra la categoría actual, con la diferencia de cuota en pesos.
- **Tabla completa** — las 11 categorías, filtrables por tipo de actividad.
- **Cómo y cuándo** — fechas de vencimiento y pasos para recategorizarse en AFIP.

## Actualizar los valores

Cada recategorización ARCA publica una tabla nueva. Hay que reemplazarla en dos lugares:

- `monotributo.c` → la constante `TABLA`
- `docs/index.html` → la constante `TABLA` del `<script>` (mismos campos, mismo orden)
