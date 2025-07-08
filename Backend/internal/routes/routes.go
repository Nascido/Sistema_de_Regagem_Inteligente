package routes

import (
    "net/http"
    "github.com/go-chi/chi/v5"
    "meuprojeto/internal/handlers"
)

func SetupRouter() http.Handler {
    r := chi.NewRouter()
    r.Get("/status", handlers.GetStatus)
    r.Get("/set_umidade", handlers.SetUmidade)
    r.Get("/restart", handlers.RestartESP)
    return r
}
