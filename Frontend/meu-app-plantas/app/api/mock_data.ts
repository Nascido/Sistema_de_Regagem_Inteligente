// Este arquivo simula a resposta que receberíamos da API da Perenual.
// Usaremos esses dados para a demonstração.

export const DADOS_DEMO = {
  id: 1,
  common_name: 'Planta de Demonstração',
  scientific_name: ['Plantae demonstratum'],
  watering: 'Average', // Necessidade de rega "Média"
  sunlight: ['full_sun', 'part_shade'], // Gosta de sol pleno ou meia-sombra
  default_image: {
    thumbnail: 'https://encrypted-tbn1.gstatic.com/images?q=tbn:ANd9GcSYlCenUZLlhRjnV551Bi8JuJvSXjV_ITnmZh6TdWn6887DisIYpaqIXgFmONuZzY2HpfeHGjE1KVlvYPS0L3LHnw', // Uma imagem de exemplo
  },
};