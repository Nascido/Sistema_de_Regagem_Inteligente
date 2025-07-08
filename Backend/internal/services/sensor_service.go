package services

import (
    "log"
    "meuprojeto/internal/models"
)

func ProcessSensorData(data models.SensorData) {
    // Exemplo: persista em banco de dados ou log
    log.Printf("Recebido: Umidade=%d, LDR=%d", data.Umidade, data.LDR)
}
