class Instrument
{
String title;

ArrayList<Integer> topo = new ArrayList<Integer>();
ArrayList<Integer> list_of_values = new ArrayList<Integer>();

int average_smooth;
int activation_thresh;
int cc_val;
float cc_increase;
float cc_decay;

JSONObject json;

Plot plot = new Plot(0, 16, 0, 1, 320, 40);
Plot cc_plot = new Plot(0, 200, 0, 127, 300, 20);

Instrument(String title_)
{
        title = title_;
}

Instrument()
{

}

void record_value(int new_value, int num_of_values) // appends new value to list of values and shifts list
{
        if (list_of_values.size() > num_of_values)
                list_of_values.remove(0);
        list_of_values.add(new_value);

        cc_plot.updateValues(list_of_values, false);
}

}
