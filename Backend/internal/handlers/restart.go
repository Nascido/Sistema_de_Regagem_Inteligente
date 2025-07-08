package handlers

import (
    "net/http"
    "meuprojeto/internal/services"
)

func RestartESP(w http.ResponseWriter, r *http.Request) {
    err := services.RestartESP()
    if err != nil {
        http.Error(w, "Falha ao reiniciar ESP32", http.StatusInternalServerError)
        return
    }
    w.Write([]byte("Reiniciando ESP32..."))
}
