
int alta_subscripcion_tema(const char *tema);

int baja_subscripcion_tema(const char *tema);

/* para la version inicial solo se usa el primer argumento dejando los restantes a NULL */
int inicio_subscriptor(void (*notif_evento)(const char *, const char *),
                void (*alta_tema)(const char *),
                void (*baja_tema)(const char *));
                           
/* solo para la version avanzada */
int fin_subscriptor();     
