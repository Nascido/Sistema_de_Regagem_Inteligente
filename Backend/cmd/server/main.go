package main

import (
    "log"
    "net/http"
    "os"

    "github.com/joho/godotenv"
    "meuprojeto/config"
    "meuprojeto/internal/routes"
)

func main() {
    // Carrega variáveis de ambiente do arquivo .env
    _ = godotenv.Load()

    cfg := config.LoadConfig()

    r := routes.SetupRouter()

    log.Printf("Servidor iniciado na porta %s", cfg.Port)
    log.Fatal(http.ListenAndServe(":"+cfg.Port, r))
}
