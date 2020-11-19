class Grid
{
boolean init = true;
int xPos;
int yPos;

int horizontal_spacing;
int vertical_spacing = height/16;
// int[] x_vertices = new int[16][16];
// int[] y_vertices = new int[16][16];
PVector[][] vertices = new PVector[16][16];

int y_distort_factor = 1;

// color:
int field_hue = 0;
int field_sat = 200;
int field_bri = 200;

float y_offset = 0;
float x_offset = 0;

Grid(int upper_left_x, int upper_left_y)
{
        xPos = upper_left_x;
        yPos = upper_left_y;
        horizontal_spacing = xPos/16;
}


void create_vertices(int num_of_colums, int num_of_rows)
{
        // calculate vertices:
        for (int i = 0; i<num_of_colums; i++)
        {
                int x = xPos + i * horizontal_spacing;         // draw from right to left
                if (i % 2 == 0)                 // even numbers: take values from snare
                        vertical_spacing = (height+2*vertical_spacing)/num_of_rows + snare.topo.get(i) * y_distort_factor;
                else                         // odd numbers: take values from hihat:
                        vertical_spacing = (height+2*vertical_spacing)/num_of_rows + hihat.topo.get(i) * y_distort_factor;

                for (int j = 0; j<num_of_rows; j++)
                {
                        int y = (j * vertical_spacing) - vertical_spacing;
                        vertices[i][j] = new PVector(x,y);
                }
                vertices[i][num_of_rows-1] = new PVector(x, height); // set last row's values
        }
        // draw vertices:
        // reg -= kick.cc_decay;
        // reg = max(reg, score.average_smooth);         // TODO: link this to overall regularity
}

void draw()
{
        y_offset = kick.cc_val;
        x_offset = sin(radians(ride.cc_val/127*180)) * crash1.cc_val;

        int iterator_ = 0;
        for (int x = 0; x<15; x++)
        {
                for (int y = 0; y<15; y++)
                {
                        colorMode(HSB);
                        if (score.ready) field_hue = (field_hue+10) % 249;           // makes crazy color effects
                        if (!score.ready) field_hue = (field_hue+125) % 250;           // two-colored pattern
                        color col;
                        if (iterator_ % 2 == 0) col = color(200);
                        else col = color(0);
                        // noFill();
                        // fill(col);
                        fill(field_hue, field_sat, field_bri);
                        noStroke();

                        beginShape();
                        vertex(vertices[x][y].x-x_offset, vertices[x][y].y-y_offset);
                        vertex(vertices[x+1][y].x-x_offset, vertices[x+1][y].y);
                        vertex(vertices[x+1][y+1].x-x_offset, vertices[x+1][y+1].y);
                        vertex(vertices[x][y+1].x, vertices[x][y+1].y);
                        endShape();

                        iterator_++;

                }
        }
        if (!score.ready) field_hue = (field_hue+125) % 250;         // do this one more time to have it repeat evenly
}

}
