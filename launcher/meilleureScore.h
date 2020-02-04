typedef struct Score_s {
  char *username;
  int score;
  int gameID;
} Score_t ;

typedef struct MeilleureScore_s {
  Score_s *score;
  int nb_element;
} MeilleureScore_t ;
