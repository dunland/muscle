// ------------------------ KEYS ---------------------------
void keyPressed()
{
        switch (key) {
        case 'a':
                list_of_circles.add(new Circle(width*2/3, height*1/5));
                println("a pressed.");
                //println("remaining objects: " + (list_of_circles.size() + list_of_lines.size()));
                break;

        case 's':
                list_of_lines.add(new Line(width*2/3));
                println("s pressed.");
                //println("remaining objects: " + (list_of_circles.size() + list_of_lines.size()));
                for (Circle circle : list_of_circles)
                {
                        circle.stopped = true;
                }
                break;

        case 'd':
                println("d pressed.");
                break;

        case 'e':
                println("e pressed.");
                grid.y_offset = max(grid.y_offset, score.average_smooth);
                grid.y_offset += 30;
                break;
        }
}
