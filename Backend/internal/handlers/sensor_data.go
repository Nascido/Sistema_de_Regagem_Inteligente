package handlers

import (
    "encoding/json"
    "io"
    "net/http"

    "meuprojeto/internal/models"
)

func ReceiveSensorData(w http.ResponseWriter, r *http.Request) {
    var data models.SensorData

    body, err := io.ReadAll(r.Body)
    if err != nil {
        http.Error(w, "Erro ao ler corpo da requisição", http.StatusBadRequest)
        return
    }
    defer r.Body.Close()

    if err := json.Unmarshal(body, &data); err != nil {
        http.Error(w, "JSON inválido", http.StatusBadRequest)
        return
    }

    // Aqui você pode chamar um service para persistir/processar os dados
    // Exemplo: services.ProcessSensorData(data)

    w.Header().Set("Content-Type", "application/json")
    w.WriteHeader(http.StatusOK)
    w.Write([]byte(`{"status":"ok"}`))
}
