# PROYECTO_CLAN

# Clan del Oriente - Sistema de Gestión de Líderes y Sucesión

## Descripción del Proyecto
Este proyecto digitaliza y automatiza los registros genealógicos del clan que ha protegido al shogun por generaciones. El sistema convierte datos históricos en una estructura de árbol binario, gestiona la línea de sucesión y automatiza la selección de nuevos líderes según las tradiciones del clan.

## Requisitos Funcionales
1. **Carga de datos**: Importación desde archivos CSV con información de miembros del clan
2. **Estructura de datos**: Conversión a árbol binario
3. **Gestión de sucesión**:
   - Visualización de la línea de sucesión actual
   - Asignación automática de nuevo líder según reglas específicas
4. **Modificación de datos**: Actualización de información de miembros (excepto ID y ID del padre)
5. **Contribuidores**: Gestión de colaboradores asociados a cada líder, con ordenamiento por grado de contribución y edad

## Estructura del Proyecto
```
PROYECTO_CLAN/
├── bin/                
│   ├── clan_data.csv    
│   ├── contributors.csv 
├── src/                
│   ├── main.cpp         
|   ├── tree/
│        ├── tree.h
|   ├── node/
|        ├── node.h
|        ├── node.cpp          
└── README.md          
```


## Reglas de Sucesión
El sistema implementa automáticamente las complejas reglas de sucesión del clan:
1. Si el lider murió y tiene hijos, el lider es el primer hijo varon vivo encontrado en su árbol.
2. Si el lider murió y solo tiene hembras, el lider es el primer hijo varon de la primera hija viva encontrado en su árbol.
3. Si el lider murió y no tiene hijos, el lider es el primer hijo varon vivo encontrado del árbol de su hermano/hermana.
4. Si el hermano/hermana esta vivo/a y no tiene hijos, el hermano/hermana se vuelve lider.
5. Si el lider murió y no tiene hermanos ni hijos, el lider es el primer hijo varon vivo encontrado del árbol de su tío/a.
6. Si su tío/a esta vivo/a y no tiene hijos, su tío/a se vuelve lider.
7. Si el lider murió y no se puede hallar ni su tío ni su hermano, buscar el primer ancestro con dos hijos y elegir como lider el primer hijo varon vivo del árbol del ancestro.

## Ejemplo de Datos
El proyecto incluye archivos CSV de ejemplo en la carpeta `bin/` con:
- Datos genealógicos del clan (`clan_data.csv`)
- Información de contribuidores (`ontributors.csv`)

## Contribución
Este proyecto fue desarrollado por Luis ALvarez siguiendo las normas de:
- Estructura de carpetas especificada
- Convención de commits
- Implementación manual de estructuras dinámicas
