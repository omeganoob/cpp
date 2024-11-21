#include <cstdio>
#include <cstdlib>

using Box = struct Box
{
    bool used{false};
    char stuff[64];
};

Box g_Boxes[100] = {0};

void print_box(u_int32_t i)
{
    printf("Box %d: %s\n", i, g_Boxes[i].stuff);
}

void edit_box(u_int32_t i)
{
    if(!g_Boxes[i].used)
    {
        printf("WHAT'S IN THE BOX?: ");
        scanf("%64s", g_Boxes[i].stuff);
        g_Boxes[i].used = true;
    }
    else
    {
        printf("Box is aleady occupied!\n");
    }
}

int read_int(u_int32_t* out)
{
    u_int32_t i = 0;
    int n_parsed = scanf("%u", &i);
    if(n_parsed > 0)
    {
        *out = i;
    }

    return n_parsed;
}

void print_menu()
{
    printf("INVENTORY SIMULATOR 1.0\n");
    printf("\tp: print a box\n");
    printf("\te: edit a box\n");
    printf("\tq: quit\n");
}

int main(int, char**)
{
    char selection = 'a';
    print_menu();
    while(selection != 'q')
    {
        selection = getchar();
        if(selection == '\n')
        {
            continue;
        }

        u_int32_t box = 0;
        switch (selection)
        
        {
        case 'q':
            break;
        case 'p':
            box = 0;
            printf("Box ID: ");
            if(!read_int(&box))
            {
                printf("Please input an integer.\n");
                continue;
            }
            print_box(box);
            break;
        case 'e':
            box = 0;
            printf("Box ID: ");
            if(!read_int(&box))
            {
                printf("Please input an interger.\n");
                continue;
            }
            edit_box(box);
            break;
        case '\n':
            break;
        default:
            printf(":(\n");
            break;
        }
    }
    return 0;
}