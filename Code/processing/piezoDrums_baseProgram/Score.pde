class Score
{
  ArrayList<Integer> notes = new ArrayList<Integer>();
  ArrayList<Integer> topo = new ArrayList<Integer>();

  int elapsedMillis;
  int current_beat_pos;
  int step;
  int note;
  int average_smooth;
  int activation_thresh;

  boolean ready = false;

  JSONObject json;

Score()
{
}

}
