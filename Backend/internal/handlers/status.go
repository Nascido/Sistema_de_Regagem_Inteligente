package handlers

import (
    "encoding/json"
    "net/http"
    "meuprojeto/internal/services"
)

func GetStatus(w http.ResponseWriter, r *http.Request) {
    status, err := services.GetESPStatus()
    if err != nil {
        http.Error(w, "Erro ao obter status do ESP32", http.StatusInternalServerError)
        return
    }
    w.Header().Set("Content-Type", "application/json")
    json.NewEncoder(w).Encode(status)
}
