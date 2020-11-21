class Instrument
{
String title;

ArrayList<Integer> topo = new ArrayList<Integer>();
ArrayList<Float> list_of_values = new ArrayList<Float>();
ArrayList<String> list_of_strings = new ArrayList<String>();

int average_smooth;
int activation_thresh;
float cc_val;
float cc_increase;
float cc_decay;
String effect;

JSONObject json;

Plot plot = new Plot(0, 16, 0, 1, 320, 40);
Plot cc_plot = new Plot(0, 200, 0, 127, 150, 20);

Instrument(String title_)
{
        title = title_;
}

Instrument()
{

}

void record_value(float new_value, int num_of_values) // appends new value to list of values and shifts list
{
        if (list_of_values.size() > num_of_values)
                list_of_values.remove(0);
        list_of_values.add(new_value);

        cc_plot.updateFloatValues(list_of_values, false);
}

void record_String(String new_String, int num_of_values) // appends new value to list of values and shifts list
{
        if (list_of_strings.size() > num_of_values)
                list_of_strings.remove(0);
        list_of_strings.add(new_String);
}

// draw a vertical line of strings from strokes:
void draw_strings(int x_position, int vertical_spacing)
{
  for (int i = 0; i<list_of_strings.size(); i++)
  {
    int y = (height-10 - i * vertical_spacing);
    int x = x_position;

    textAlign(TOP, LEFT);
    fill(255);
    text(list_of_strings.get(i), x, y);
  }
}

}
