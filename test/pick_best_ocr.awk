{
  best_score = 0;
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
    start_time = "0:"int($$1/60)":"$$1%60;
    end_time = "0:"int($$2/60)":"$$2%60;
    printf("%s %s %s\n", start_time, end_time, line);
  }
}
