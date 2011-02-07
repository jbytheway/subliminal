{
  best_score = -1e80; # Minus infinity
  for (i = 3; i <= NF; ++i) {
    text_file = gensub(/png/, "txt", "g", $i);
    command = scorer" < "text_file;
    command | getline score;
    close(command);
    if (score > best_score) {
      best_file = text_file;
      best_score = score;
    }
  }
  if (best_file != "") {
    command="< "best_file" tr '\n' ' ' | tr '\\{}' ___";
    command | getline line;
    close(command);
    printf("%s %s %s\n", $1, $2, line);
  }
}
