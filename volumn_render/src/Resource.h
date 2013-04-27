#include "GL/glew.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <io.h>
#include "hsv\hsv.h"
#include <process.h>
#include "GL/glut.h"
#include "inputModule.h"
#include "control_panel.h"
#include "ply.h"
#include "FreeImage.h"
#include <list>
using namespace std;

#define TRANSFERVIEW_POSX 100
#define TRANSFERVIEW_POSY 200
#define TRANSFERVIEW_WIDTH 256
#define HISTORGRAM_POSX 100
#define HISTORGRAM_POSY 500
#define PRFER_COLOR_TILE_NUM 6
