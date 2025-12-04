# SO-Tarea-3
María de los Ángeles y Cristóbal Navarro

# Ejecución
1. Situarse en la carpeta donde esta el main
2. Usar el siguiente comando "g++ -std=c++11 main.cpp -o main
3. Ejecutar usando ./main
4. Introducir tamaño de la memoria fisica
5. Introducir tamaño de las paginas
6. Evaluar

# Funcionamiento

Este programa implementa una simulación de memoria virtual utilizando paginación, asignación de procesos, reemplazo FIFO, múltiples hilos y registro detallado en archivo de log.

---

# Estructura General

- Se simulan páginas y marcos a través de la misma clase `pagina`.
- La simulación corre con múltiples hilos:
  - creación de procesos,
  - eliminación de procesos,
  - consultas a direcciones virtuales,
  - renderizado periódico del estado de la memoria.
- El archivo `consultas.log` almacena:
  - consultas realizadas,
  - reemplazos de marcos,
  - estados previos y posteriores a cada operación.

---

# Componentes Principales

## Clase `pagina`
Representa tanto una página virtual como un marco físico.

Atributos:
- estado libre/ocupado,
- id del proceso asignado,
- número de parte del proceso,
- porcentaje de uso.

Métodos:
- `ocupar()`: asigna datos a la página o marco,
- `liberar()`: marca como libre nuevamente.

## Clase `proceso`
Solo almacena:
- tamaño total del proceso,
- id único autoincremental.

---

# Lógica de Asignación

## Creación de procesos
- Un proceso se divide en páginas según el tamaño definido.
- Se recorre la memoria virtual:
  - cada página libre se ocupa,
  - se intenta asignar también un marco físico disponible.
- Si no hay suficientes páginas libres para completar el proceso:
  - la simulación se detiene por falta de memoria.

---

# Reemplazo FIFO

## Función `reemplazarMarco()`
- Si existe un marco libre, se usa el primero.
- Si no, se aplica reemplazo FIFO usando el índice `fifo`.
- Luego del reemplazo:
  - se registra en `consultas.log`,
  - se avanza el índice FIFO circularmente.

---

# Consultas a Direcciones

## Ciclo consultador
- La primera consulta se fuerza fuera de rango para provocar un reemplazo.
- Después:
  - se genera una dirección virtual válida,
  - se calcula su número de página,
  - se registra en el log con:
    - dirección consultada,
    - número de página,
    - proceso y parte asociados.
  - se renderiza el estado previo y posterior,
  - se ejecuta reemplazo FIFO.

---

# Eliminación de Procesos

## Ciclo eliminador
- Espera 30 segundos antes de la primera eliminación.
- Selecciona qué proceso eliminar:
  - primera vez: el que esté fuera de rango,
  - luego: un proceso aleatorio.
- Libera todas sus páginas virtuales y sus marcos físicos.
- Repite cada 5 segundos.

---

# Renderizado

## Render en consola (`render()`)
- Muestra una tabla con:
  - páginas virtuales,
  - marcos físicos.
- Utiliza colores ANSI para identificar procesos.

## Render a log (`renderMSG()`)
- Escribe el mismo estado en `consultas.log`,
- Formato de tabla alineada sin colores.

---

# Hilos de Ejecución

- `loop_procesos()`: crea procesos cada 2 segundos.
- `loop_render()`: renderiza el estado cada 500 ms.
- `loop_eliminador()`: elimina procesos cada 5 segundos.
- `loop_consultador()`: inicia tras 30 s y consulta direcciones periódicamente.

Los hilos se detienen cuando:
- la memoria virtual se llena,
- algún proceso no puede ser asignado completamente.