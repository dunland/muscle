class Instrument
{
String title;

ArrayList<Integer> topo = new ArrayList<Integer>();
ArrayList<Float> list_of_values = new ArrayList<Float>();

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

}
