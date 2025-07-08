package routes

import (
    "net/http"

    "github.com/go-chi/chi/v5"
    "meuprojeto/internal/handlers"
)

func SetupRouter() http.Handler {
    r := chi.NewRouter()
    r.Post("/api/sensor-data", handlers.ReceiveSensorData)
    return r
}
