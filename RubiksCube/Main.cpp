/* SCE CONFIDENTIAL
 * PlayStation(R)Vita Programmer Tool Runtime Library Release 02.000.081
 * Copyright (C) 2010 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

// All fonts rel�ated stuff hs been ripped out.

/*	

	This sample shows how to initialize libdbgfont (and libgxm),
	and render debug font with triangle for clear the screen.

	This sample is split into the following sections:

		1. Initialize libdbgfont
		2. Initialize libgxm
		3. Allocate display buffers, set up the display queue
		4. Create a shader patcher and register programs
		5. Create the programs and data for the clear
		6. Start the main loop
			7. Update step
			8. Rendering step
			9. Flip operation and render debug font at display callback
		10. Wait for rendering to complete
		11. Destroy the programs and data for the clear triangle
		12. Finalize libgxm
		13. Finalize libdbgfont

	Please refer to the individual comment blocks for details of each section.
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sceerror.h>
#include <gxm.h>
#include <kernel.h>
#include <ctrl.h>
#include <display.h>
#include <libdbg.h>


#include <libdbgfont.h>
#include <math.h>

#include <vectormath.h>

#include "Logic.h"
#include "Side.h"
#include "Cube.h"
#include "Animator.h"
#include <iostream>
#include "RenderAssistant.h"
#include "InputSystem.h"
#include <touch.h>

#include <libsysmodule.h>

using namespace sce::Vectormath::Simd::Aos;

/*	Define the debug font pixel color format to render to. */
#define DBGFONT_PIXEL_FORMAT		SCE_DBGFONT_PIXELFORMAT_A8B8G8R8


/*	Define the width and height to render at the native resolution */
#define DISPLAY_WIDTH				960
#define DISPLAY_HEIGHT				544
#define DISPLAY_STRIDE_IN_PIXELS	1024

/*	Define the libgxm color format to render to.
	This should be kept in sync with the display format to use with the SceDisplay library.
*/
#define DISPLAY_COLOR_FORMAT		SCE_GXM_COLOR_FORMAT_A8B8G8R8
#define DISPLAY_PIXEL_FORMAT		SCE_DISPLAY_PIXELFORMAT_A8B8G8R8

/*	Define the number of back buffers to use with this sample.  Most applications
	should use a value of 2 (double buffering) or 3 (triple buffering).
*/
#define DISPLAY_BUFFER_COUNT		3

/*	Define the maximum number of queued swaps that the display queue will allow.
	This limits the number of frames that the CPU can get ahead of the GPU,
	and is independent of the actual number of back buffers.  The display
	queue will block during sceGxmDisplayQueueAddEntry if this number of swaps
	have already been queued.
*/
#define DISPLAY_MAX_PENDING_SWAPS	2


/*	Helper macro to align a value */
#define ALIGN(x, a)					(((x) + ((a) - 1)) & ~((a) - 1))


/*	The build process for the sample embeds the shader programs directly into the
	executable using the symbols below.  This is purely for convenience, it is
	equivalent to simply load the binary file into memory and cast the contents
	to type SceGxmProgram.
*/
extern const SceGxmProgram binaryClearVGxpStart;
extern const SceGxmProgram binaryClearFGxpStart;

/*	Data structure for clear geometry */
typedef struct ClearVertex
{
	float x;
	float y;
} ClearVertex;


// !! Data related to rendering vertex.
extern const SceGxmProgram binaryBasicVGxpStart;
extern const SceGxmProgram binaryBasicFGxpStart;

/*	Data structure for basic geometry */


/*	Data structure to pass through the display queue.  This structure is
	serialized during sceGxmDisplayQueueAddEntry, and is used to pass
	arbitrary data to the display callback function, called from an internal
	thread once the back buffer is ready to be displayed.

	In this example, we only need to pass the base address of the buffer.
*/
typedef struct DisplayData
{
	void *address;
} DisplayData;

static SceGxmContextParams		s_contextParams;			/* libgxm context parameter */
static SceGxmRenderTargetParams s_renderTargetParams;		/* libgxm render target parameter */
static SceGxmContext			*s_context			= NULL;	/* libgxm context */
static SceGxmRenderTarget		*s_renderTarget		= NULL;	/* libgxm render target */
static SceGxmShaderPatcher		*s_shaderPatcher	= NULL;	/* libgxm shader patcher */

/*	display data */
static void							*s_displayBufferData[ DISPLAY_BUFFER_COUNT ];
static SceGxmSyncObject				*s_displayBufferSync[ DISPLAY_BUFFER_COUNT ];
static int32_t						s_displayBufferUId[ DISPLAY_BUFFER_COUNT ];
static SceGxmColorSurface			s_displaySurface[ DISPLAY_BUFFER_COUNT ];
static uint32_t						s_displayFrontBufferIndex = 0;
static uint32_t						s_displayBackBufferIndex = 0;
static SceGxmDepthStencilSurface	s_depthSurface;

/*	shader data */
static int32_t					s_clearVerticesUId;
static int32_t					s_clearIndicesUId;
static SceGxmShaderPatcherId	s_clearVertexProgramId;
static SceGxmShaderPatcherId	s_clearFragmentProgramId;
// !! Shader pactcher addded.
static SceGxmShaderPatcherId	s_basicVertexProgramId;
static SceGxmShaderPatcherId	s_basicFragmentProgramId;
static SceUID					s_patcherFragmentUsseUId;
static SceUID					s_patcherVertexUsseUId;
static SceUID					s_patcherBufferUId;
static SceUID					s_depthBufferUId;
static SceUID					s_vdmRingBufferUId;
static SceUID					s_vertexRingBufferUId;
static SceUID					s_fragmentRingBufferUId;
static SceUID					s_fragmentUsseRingBufferUId;
static ClearVertex				*s_clearVertices			= NULL;
static uint16_t					*s_clearIndices				= NULL;
static SceGxmVertexProgram		*s_clearVertexProgram		= NULL;
static SceGxmFragmentProgram	*s_clearFragmentProgram		= NULL;
// !! Data added.
static SceGxmVertexProgram		*s_basicVertexProgram		= NULL;
static SceGxmFragmentProgram	*s_basicFragmentProgram		= NULL;
static BasicVertex				*s_basicVertices			= NULL;
static uint16_t					*s_basicIndices				= NULL;
static int32_t					s_basicVerticesUId;
static int32_t					s_basicIndiceUId;


//!! The program parameter for the transformation of the triangle
static Matrix4 s_View;
static Matrix4 s_Projection;
static Matrix4 s_globalRotation;
static Quat rotationQuat = Quat::identity();
static const SceGxmProgramParameter *s_wvpParam = NULL;
static const SceGxmProgramParameter *s_wParam = NULL;


/* Callback function to allocate memory for the shader patcher */
static void *patcherHostAlloc( void *userData, uint32_t size );

/* Callback function to allocate memory for the shader patcher */
static void patcherHostFree( void *userData, void *mem );

/*	Callback function for displaying a buffer */
static void displayCallback( const void *callbackData );




/*	@brief Main entry point for the application
	@return Error code result of processing during execution: <c> SCE_OK </c> on success,
	or another code depending upon the error
*/
int main( void );


// !! Here we create the matrix.
void Update(void);

void UpdateRotation(float angleY, float angleX);



/*	@brief Initializes the graphics services and the libgxm graphics library
	@return Error code result of processing during execution: <c> SCE_OK </c> on success,
	or another code depending upon the error
*/
static int initGxm( void );

/*	 @brief Creates scenes with libgxm */
static void createGxmData( void );


/*	@brief Main rendering function to draw graphics to the display */
static void render( void );

/*	@brief render libgxm scenes */
static void renderGxm( void );



/*	@brief cycle display buffer */
static void cycleDisplayBuffers( void );

/*	@brief Destroy scenes with libgxm */
static void destroyGxmData( void );


/*	@brief Function to shut down libgxm and the graphics display services
	@return Error code result of processing during execution: <c> SCE_OK </c> on success,
	or another code depending upon the error
*/
static int shutdownGxm( void );

float makeValue(SceUInt8 input);

void colorSide(Side* side);
void allBlack();
void colorCube();

/*	@brief User main thread parameters */
extern const char			sceUserMainThreadName[]		= "simple_main_thr";
extern const int			sceUserMainThreadPriority	= SCE_KERNEL_DEFAULT_PRIORITY_USER;
extern const unsigned int	sceUserMainThreadStackSize	= SCE_KERNEL_STACK_SIZE_DEFAULT_USER_MAIN;

/*	@brief libc parameters */
unsigned int	sceLibcHeapSize	= 1*1024*1024;
float deltaTime;

Logic *logic;
const unsigned int NUM_CUBES = 27;
Cube* cubeList[NUM_CUBES];
Animator *animator;
InputSystem *input;


/* Main entry point of program */
int main( void )
{
	
	sceSysmoduleLoadModule(SCE_SYSMODULE_RAZOR_CAPTURE);

	int returnCode = SCE_OK;
	bool wasXButton;
	SceCtrlData result;
	/* initialize libdbgfont and libgxm */
	returnCode =initGxm();
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

    SceDbgFontConfig config;
	memset( &config, 0, sizeof(SceDbgFontConfig) );
	config.fontSize = SCE_DBGFONT_FONTSIZE_LARGE;

	returnCode = sceDbgFontInit( &config );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

	/* Message for SDK sample auto test */
	printf( "## simple: INIT SUCCEEDED ##\n" );

	logic = new Logic();
	for(int i = 0; i < 27; i++){
		cubeList[i] = new Cube();
	}

	animator = new Animator(cubeList, logic);
	input = new InputSystem(animator);

	//Schalte Backtouch Panel ein
	sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);


	int i = 0;
	//0 - 8 front

	while(i < 27){
		for(int offSetZ = -1; offSetZ < 2; offSetZ++){
			for(int offSetY = -1; offSetY < 2; offSetY++){
				for(int offSetX = -1; offSetX < 2; offSetX++){
					Vector3 pos(1.1*offSetX, 1.1*offSetY, 1.1*offSetZ);
					cubeList[i]->modelTransformation = Matrix4::translation(pos);
					i++;
				}
			}
		}
	}

	allBlack();

	//F�rbe die Seiten
	colorSide(logic->sides[Logic::SIDE_FRONT]);
	colorSide(logic->sides[Logic::SIDE_LEFT]);
	colorSide(logic->sides[Logic::SIDE_RIGHT]);
	colorSide(logic->sides[Logic::SIDE_BACK]);
	colorSide(logic->sides[Logic::SIDE_TOP]);
	colorSide(logic->sides[Logic::SIDE_BOTTOM]);

	/* create gxm graphics data */
	createGxmData();

     // Set sampling mode for input device.
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_DIGITALANALOG_WIDE);

	
	//logic->RotateFront();

	/* 6. main loop */
	while (true)
	{
		colorCube();

		deltaTime = 1.0f/60.0f;
        Update();
		animator->update(deltaTime);
		input->Update();
		render();
        sceDbgFontPrint( 20, 20, 0xffffffff, "Rubik's Cube" );
		cycleDisplayBuffers();
	}


    /*
	// 10. wait until rendering is done 
	sceGxmFinish( s_context );

	// destroy gxm graphics data 
	destroyGxmData();

	// shutdown libdbgfont and libgxm 
	returnCode = shutdownGxm();
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

	// Message for SDK sample auto test
	printf( "## api_libdbgfont/simple: FINISHED ##\n" );

	return returnCode;
    */
}

float makeValue(SceUInt8 input){
    float result = (float)input;
    result = (result*2.0f) / 255.0f-1.0f;
    return result;
 
}


float makeFloat(unsigned char input)
{
	float res = (((float)(input)) / 255.0f * 2.0f) - 1.0f;

    if ((float)abs(res) < 0.1f)
	{
		return 0.0f;
	}
	else
	{
		return res;
	}
}

static float s_accumulatedTurningAngleX;
static float s_accumulatedTurningAngleY;
SceTouchData resultTouchStart;

void Update (void)
{
	 SceTouchData resultTouch;
	 sceTouchRead(SCE_TOUCH_PORT_BACK, &resultTouch, 1);
	 float xDif = 0.0f;
	 float yDif = 0.0f;
	 if(resultTouch.report[0].id != resultTouchStart.report[0].id)
	 {
		 resultTouchStart = resultTouch;
	 }

	 if(resultTouch.reportNum)
	 {
		xDif = resultTouch.report[0].x - resultTouchStart.report[0].x;
		yDif = resultTouch.report[0].y - resultTouchStart.report[0].y;
	 }
	 

     SceCtrlData result;
     sceCtrlReadBufferPositive(0, &result, 1);

     //s_accumulatedTurningAngleX += makeFloat(result.lx) * 0.03f;
     //s_accumulatedTurningAngleY += makeFloat(result.ly) * 0.03f;
	 


	 UpdateRotation(makeFloat(result.ly), makeFloat(result.lx));
	 UpdateRotation(yDif * 0.01f, xDif * 0.01f);

     s_globalRotation = Matrix4::rotation(rotationQuat);

     Matrix4 lookAt = Matrix4::lookAt(Point3(0.0f, 0.0f, -10.0f), Point3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f));

     Matrix4 perspective = Matrix4::perspective(3.141592f / 4.0f,
		(float)DISPLAY_WIDTH/(float)DISPLAY_HEIGHT,
		1.0f, //Nearplane
		100.0f); //Farplane

     s_View = lookAt;
	 s_Projection = perspective;
};

void allBlack()
{
	for(int i = 0; i < 27; i++)
	{
		cubeList[i]->setColor(0x00000000, Logic::SIDE_FRONT);
		cubeList[i]->setColor(0x00000000, Logic::SIDE_BACK);
		cubeList[i]->setColor(0x00000000, Logic::SIDE_LEFT);
		cubeList[i]->setColor(0x00000000, Logic::SIDE_RIGHT);
		cubeList[i]->setColor(0x00000000, Logic::SIDE_TOP);
		cubeList[i]->setColor(0x00000000, Logic::SIDE_BOTTOM);
	}
}

void colorSide(Side *side){
	int counter = 0;

	if (side->logicalSide == Logic::SIDE_FRONT)
	{
		for(int i = 0; i <= 8; i++)
		{
			int matrixIndexI, matrixIndexJ;
			matrixIndexJ = i%3;
			matrixIndexI = i/3;

			uint32_t color = Side::getColorValue(side->faces[matrixIndexI][matrixIndexJ].color);
			cubeList[i]->setColor(color, Logic::SIDE_FRONT);		
		}
	}
	else if(side->logicalSide == Logic::SIDE_LEFT)
	{
			cubeList[0]->setColor(Side::getColorValue(side->faces[0][2].color), Logic::SIDE_LEFT);
			cubeList[3]->setColor(Side::getColorValue(side->faces[1][2].color), Logic::SIDE_LEFT);
			cubeList[6]->setColor(Side::getColorValue(side->faces[2][2].color), Logic::SIDE_LEFT);
												   	 						 
			cubeList[9]->setColor(Side::getColorValue(side->faces[0][1].color), Logic::SIDE_LEFT);
			cubeList[12]->setColor(Side::getColorValue(side->faces[1][1].color), Logic::SIDE_LEFT);
			cubeList[15]->setColor(Side::getColorValue(side->faces[2][1].color), Logic::SIDE_LEFT);
																			 
			cubeList[18]->setColor(Side::getColorValue(side->faces[0][0].color), Logic::SIDE_LEFT);
			cubeList[21]->setColor(Side::getColorValue(side->faces[1][0].color), Logic::SIDE_LEFT);
			cubeList[24]->setColor(Side::getColorValue(side->faces[2][0].color), Logic::SIDE_LEFT); 
	}
	else if(side->logicalSide == Logic::SIDE_RIGHT)
	{
			cubeList[2]->setColor(Side::getColorValue(side->faces[0][2].color), Logic::SIDE_RIGHT);
			cubeList[5]->setColor(Side::getColorValue(side->faces[1][2].color), Logic::SIDE_RIGHT);
			cubeList[8]->setColor(Side::getColorValue(side->faces[2][2].color), Logic::SIDE_RIGHT);
												   	 						 
			cubeList[11]->setColor(Side::getColorValue(side->faces[0][1].color), Logic::SIDE_RIGHT);
			cubeList[14]->setColor(Side::getColorValue(side->faces[1][1].color), Logic::SIDE_RIGHT);
			cubeList[17]->setColor(Side::getColorValue(side->faces[2][1].color), Logic::SIDE_RIGHT);
																			 
			cubeList[20]->setColor(Side::getColorValue(side->faces[0][0].color), Logic::SIDE_RIGHT);
			cubeList[23]->setColor(Side::getColorValue(side->faces[1][0].color), Logic::SIDE_RIGHT);
			cubeList[26]->setColor(Side::getColorValue(side->faces[2][0].color), Logic::SIDE_RIGHT); 

	}
	else if(side->logicalSide == Logic::SIDE_BACK)
	{
		for(int i = 0; i <= 8; i++)
		{
			int matrixIndexI, matrixIndexJ;
			matrixIndexJ = i%3;
			matrixIndexI = (8-i)/3;

			uint32_t color = Side::getColorValue(side->faces[matrixIndexI][matrixIndexJ].color);
			cubeList[i + 18]->setColor(color, Logic::SIDE_BACK);		
		}
		//for(int i=18; i<=26; i++)
			//cubeList[i]->setColor(0xFFFFFFFF, Logic::SIDE_BACK);
	}
	else if(side->logicalSide == Logic::SIDE_TOP)
	{
			//0 1 2	9 10 11	18 19 20
			cubeList[0]->setColor(Side::getColorValue(side->faces[2][0].color), Logic::SIDE_TOP);
			cubeList[1]->setColor(Side::getColorValue(side->faces[2][1].color), Logic::SIDE_TOP);
			cubeList[2]->setColor(Side::getColorValue(side->faces[2][2].color), Logic::SIDE_TOP);

			cubeList[9]->setColor( Side::getColorValue(side->faces[1][0].color), Logic::SIDE_TOP);
			cubeList[10]->setColor(Side::getColorValue(side->faces[1][1].color), Logic::SIDE_TOP);
			cubeList[11]->setColor(Side::getColorValue(side->faces[1][2].color), Logic::SIDE_TOP);

			cubeList[18]->setColor(Side::getColorValue(side->faces[0][0].color), Logic::SIDE_TOP);
			cubeList[19]->setColor(Side::getColorValue(side->faces[0][1].color), Logic::SIDE_TOP);
			cubeList[20]->setColor(Side::getColorValue(side->faces[0][2].color), Logic::SIDE_TOP);
	}
	else if(side->logicalSide == Logic::SIDE_BOTTOM)
	{
		// 6 7 8 15 16 17 24 25 26
		cubeList[6]->setColor(Side::getColorValue(side->faces[0][0].color), Logic::SIDE_BOTTOM);
		cubeList[7]->setColor(Side::getColorValue(side->faces[0][1].color), Logic::SIDE_BOTTOM);
		cubeList[8]->setColor(Side::getColorValue(side->faces[0][2].color), Logic::SIDE_BOTTOM);

		cubeList[15]->setColor(Side::getColorValue(side->faces[1][0].color), Logic::SIDE_BOTTOM);
		cubeList[16]->setColor(Side::getColorValue(side->faces[1][1].color), Logic::SIDE_BOTTOM);
		cubeList[17]->setColor(Side::getColorValue(side->faces[1][2].color), Logic::SIDE_BOTTOM);

		cubeList[24]->setColor(Side::getColorValue(side->faces[2][0].color), Logic::SIDE_BOTTOM);
		cubeList[25]->setColor(Side::getColorValue(side->faces[2][1].color), Logic::SIDE_BOTTOM);
		cubeList[26]->setColor(Side::getColorValue(side->faces[2][2].color), Logic::SIDE_BOTTOM);
	}
}

void colorCube()
{
	colorSide(logic->sides[Logic::SIDE_FRONT]);
	colorSide(logic->sides[Logic::SIDE_LEFT]);
	colorSide(logic->sides[Logic::SIDE_RIGHT]);
	colorSide(logic->sides[Logic::SIDE_BACK]);
	colorSide(logic->sides[Logic::SIDE_TOP]);
	colorSide(logic->sides[Logic::SIDE_BOTTOM]);
}

void UpdateRotation(float rotationY, float rotationX){
	Quat w(rotationY, -rotationX, 0.0f, 1.0);
	rotationQuat += 0.5f * w * rotationQuat * 0.25f;
	rotationQuat = normalize(rotationQuat);
}


/* Initialize libgxm */
int initGxm( void )
{
/* ---------------------------------------------------------------------
	2. Initialize libgxm

	First we must initialize the libgxm library by calling sceGxmInitialize.
	The single argument to this function is the size of the parameter buffer to
	allocate for the GPU.  We will use the default 16MiB here.

	Once initialized, we need to create a rendering context to allow to us
	to render scenes on the GPU.  We use the default initialization
	parameters here to set the sizes of the various context ring buffers.

	Finally we create a render target to describe the geometry of the back
	buffers we will render to.  This object is used purely to schedule
	rendering jobs for the given dimensions, the color surface and
	depth/stencil surface must be allocated separately.
	--------------------------------------------------------------------- */

	int returnCode = SCE_OK;

	/* set up parameters */
	SceGxmInitializeParams initializeParams;
	memset( &initializeParams, 0, sizeof(SceGxmInitializeParams) );
	initializeParams.flags = 0;
	initializeParams.displayQueueMaxPendingCount = DISPLAY_MAX_PENDING_SWAPS;
	initializeParams.displayQueueCallback = displayCallback;
	initializeParams.displayQueueCallbackDataSize = sizeof(DisplayData);
	initializeParams.parameterBufferSize = SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE;

	/* start libgxm */
	returnCode = sceGxmInitialize( &initializeParams );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

	/* allocate ring buffer memory using default sizes */
	void *vdmRingBuffer = graphicsAlloc( SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE, SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE, 4, SCE_GXM_MEMORY_ATTRIB_READ, &s_vdmRingBufferUId );

	void *vertexRingBuffer = graphicsAlloc( SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE, SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE, 4, SCE_GXM_MEMORY_ATTRIB_READ, &s_vertexRingBufferUId );

	void *fragmentRingBuffer = graphicsAlloc( SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE, SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE, 4, SCE_GXM_MEMORY_ATTRIB_READ, &s_fragmentRingBufferUId );

	uint32_t fragmentUsseRingBufferOffset;
	void *fragmentUsseRingBuffer = fragmentUsseAlloc( SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE, &s_fragmentUsseRingBufferUId, &fragmentUsseRingBufferOffset );

	/* create a rendering context */
	memset( &s_contextParams, 0, sizeof(SceGxmContextParams) );
	s_contextParams.hostMem = malloc( SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE );
	s_contextParams.hostMemSize = SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE;
	s_contextParams.vdmRingBufferMem = vdmRingBuffer;
	s_contextParams.vdmRingBufferMemSize = SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE;
	s_contextParams.vertexRingBufferMem = vertexRingBuffer;
	s_contextParams.vertexRingBufferMemSize = SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE;
	s_contextParams.fragmentRingBufferMem = fragmentRingBuffer;
	s_contextParams.fragmentRingBufferMemSize = SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE;
	s_contextParams.fragmentUsseRingBufferMem = fragmentUsseRingBuffer;
	s_contextParams.fragmentUsseRingBufferMemSize = SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE;
	s_contextParams.fragmentUsseRingBufferOffset = fragmentUsseRingBufferOffset;
	returnCode = sceGxmCreateContext( &s_contextParams, &s_context );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

	/* set buffer sizes for this sample */
	const uint32_t patcherBufferSize = 64*1024;
	const uint32_t patcherVertexUsseSize = 64*1024;
	const uint32_t patcherFragmentUsseSize = 64*1024;

	/* allocate memory for buffers and USSE code */
	void *patcherBuffer = graphicsAlloc( SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE, patcherBufferSize, 4, SCE_GXM_MEMORY_ATTRIB_WRITE|SCE_GXM_MEMORY_ATTRIB_WRITE, &s_patcherBufferUId );

	uint32_t patcherVertexUsseOffset;
	void *patcherVertexUsse = vertexUsseAlloc( patcherVertexUsseSize, &s_patcherVertexUsseUId, &patcherVertexUsseOffset );

	uint32_t patcherFragmentUsseOffset;
	void *patcherFragmentUsse = fragmentUsseAlloc( patcherFragmentUsseSize, &s_patcherFragmentUsseUId, &patcherFragmentUsseOffset );

	/* create a shader patcher */
	SceGxmShaderPatcherParams patcherParams;
	memset( &patcherParams, 0, sizeof(SceGxmShaderPatcherParams) );
	patcherParams.userData = NULL;
	patcherParams.hostAllocCallback = &patcherHostAlloc;
	patcherParams.hostFreeCallback = &patcherHostFree;
	patcherParams.bufferAllocCallback = NULL;
	patcherParams.bufferFreeCallback = NULL;
	patcherParams.bufferMem = patcherBuffer;
	patcherParams.bufferMemSize = patcherBufferSize;
	patcherParams.vertexUsseAllocCallback = NULL;
	patcherParams.vertexUsseFreeCallback = NULL;
	patcherParams.vertexUsseMem = patcherVertexUsse;
	patcherParams.vertexUsseMemSize = patcherVertexUsseSize;
	patcherParams.vertexUsseOffset = patcherVertexUsseOffset;
	patcherParams.fragmentUsseAllocCallback = NULL;
	patcherParams.fragmentUsseFreeCallback = NULL;
	patcherParams.fragmentUsseMem = patcherFragmentUsse;
	patcherParams.fragmentUsseMemSize = patcherFragmentUsseSize;
	patcherParams.fragmentUsseOffset = patcherFragmentUsseOffset;
	returnCode = sceGxmShaderPatcherCreate( &patcherParams, &s_shaderPatcher );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

	/* create a render target */
	memset( &s_renderTargetParams, 0, sizeof(SceGxmRenderTargetParams) );
	s_renderTargetParams.flags = 0;
	s_renderTargetParams.width = DISPLAY_WIDTH;
	s_renderTargetParams.height = DISPLAY_HEIGHT;
	s_renderTargetParams.scenesPerFrame = 1;
	s_renderTargetParams.multisampleMode = SCE_GXM_MULTISAMPLE_NONE;
	s_renderTargetParams.multisampleLocations	= 0;
	s_renderTargetParams.driverMemBlock = SCE_UID_INVALID_UID;

	returnCode = sceGxmCreateRenderTarget( &s_renderTargetParams, &s_renderTarget );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );


/* ---------------------------------------------------------------------
	3. Allocate display buffers, set up the display queue

	We will allocate our back buffers in CDRAM, and create a color
	surface for each of them.

	To allow display operations done by the CPU to be synchronized with
	rendering done by the GPU, we also create a SceGxmSyncObject for each
	display buffer.  This sync object will be used with each scene that
	renders to that buffer and when queueing display flips that involve
	that buffer (either flipping from or to).

	Finally we create a display queue object that points to our callback
	function.
	--------------------------------------------------------------------- */

	/* allocate memory and sync objects for display buffers */
	for ( unsigned int i = 0 ; i < DISPLAY_BUFFER_COUNT ; ++i )
	{
		/* allocate memory with large size to ensure physical contiguity */
		s_displayBufferData[i] = graphicsAlloc( SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RWDATA, ALIGN(4*DISPLAY_STRIDE_IN_PIXELS*DISPLAY_HEIGHT, 1*1024*1024), SCE_GXM_COLOR_SURFACE_ALIGNMENT, SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE, &s_displayBufferUId[i] );
		SCE_DBG_ALWAYS_ASSERT( s_displayBufferData[i] );

		/* memset the buffer to debug color */
		for ( unsigned int y = 0 ; y < DISPLAY_HEIGHT ; ++y )
		{
			unsigned int *row = (unsigned int *)s_displayBufferData[i] + y*DISPLAY_STRIDE_IN_PIXELS;

			for ( unsigned int x = 0 ; x < DISPLAY_WIDTH ; ++x )
			{
				row[x] = 0x0;
			}
		}

		/* initialize a color surface for this display buffer */
		returnCode = sceGxmColorSurfaceInit( &s_displaySurface[i], DISPLAY_COLOR_FORMAT, SCE_GXM_COLOR_SURFACE_LINEAR, SCE_GXM_COLOR_SURFACE_SCALE_NONE,
											 SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_STRIDE_IN_PIXELS, s_displayBufferData[i] );
		SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

		/* create a sync object that we will associate with this buffer */
		returnCode = sceGxmSyncObjectCreate( &s_displayBufferSync[i] );
		SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );
	}

	/* compute the memory footprint of the depth buffer */
	const uint32_t alignedWidth = ALIGN( DISPLAY_WIDTH, SCE_GXM_TILE_SIZEX );
	const uint32_t alignedHeight = ALIGN( DISPLAY_HEIGHT, SCE_GXM_TILE_SIZEY );
	uint32_t sampleCount = alignedWidth*alignedHeight;
	uint32_t depthStrideInSamples = alignedWidth;

	/* allocate it */
	void *depthBufferData = graphicsAlloc( SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE, 4*sampleCount, SCE_GXM_DEPTHSTENCIL_SURFACE_ALIGNMENT, SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE, &s_depthBufferUId );

	/* create the SceGxmDepthStencilSurface structure */
	returnCode = sceGxmDepthStencilSurfaceInit( &s_depthSurface, SCE_GXM_DEPTH_STENCIL_FORMAT_S8D24, SCE_GXM_DEPTH_STENCIL_SURFACE_TILED, depthStrideInSamples, depthBufferData, NULL );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

	return returnCode;
}


/* Create libgxm scenes */
void createGxmData( void )
{
/* ---------------------------------------------------------------------
	4. Create a shader patcher and register programs

	A shader patcher object is required to produce vertex and fragment
	programs from the shader compiler output.  First we create a shader
	patcher instance, using callback functions to allow it to allocate
	and free host memory for internal state.

	In order to create vertex and fragment programs for a particular
	shader, the compiler output must first be registered to obtain an ID
	for that shader.  Within a single ID, vertex and fragment programs
	are reference counted and could be shared if created with identical
	parameters.  To maximise this sharing, programs should only be
	registered with the shader patcher once if possible, so we will do
	this now.
	--------------------------------------------------------------------- */

	/* register programs with the patcher */
	int returnCode = sceGxmShaderPatcherRegisterProgram( s_shaderPatcher, &binaryClearVGxpStart, &s_clearVertexProgramId );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );
	returnCode = sceGxmShaderPatcherRegisterProgram( s_shaderPatcher, &binaryClearFGxpStart, &s_clearFragmentProgramId );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );


    returnCode = sceGxmShaderPatcherRegisterProgram( s_shaderPatcher, &binaryBasicVGxpStart, &s_basicVertexProgramId );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );
	returnCode = sceGxmShaderPatcherRegisterProgram( s_shaderPatcher, &binaryBasicFGxpStart, &s_basicFragmentProgramId );
    SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );


/* ---------------------------------------------------------------------
	5. Create the programs and data for the clear

	On SGX hardware, vertex programs must perform the unpack operations
	on vertex data, so we must define our vertex formats in order to
	create the vertex program.  Similarly, fragment programs must be
	specialized based on how they output their pixels and MSAA mode
	(and texture format on ES1).

	We define the clear geometry vertex format here and create the vertex
	and fragment program.

	The clear vertex and index data is static, we allocate and write the
	data here.
	--------------------------------------------------------------------- */

	/* get attributes by name to create vertex format bindings */
	const SceGxmProgram *clearProgram = sceGxmShaderPatcherGetProgramFromId( s_clearVertexProgramId );
	SCE_DBG_ALWAYS_ASSERT( clearProgram );
	const SceGxmProgramParameter *paramClearPositionAttribute = sceGxmProgramFindParameterByName( clearProgram, "aPosition" );
	SCE_DBG_ALWAYS_ASSERT( paramClearPositionAttribute && ( sceGxmProgramParameterGetCategory(paramClearPositionAttribute) == SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE ) );

	/* create clear vertex format */
	SceGxmVertexAttribute clearVertexAttributes[1];
	SceGxmVertexStream clearVertexStreams[1];
	clearVertexAttributes[0].streamIndex = 0;
	clearVertexAttributes[0].offset = 0;
	clearVertexAttributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	clearVertexAttributes[0].componentCount = 2;
	clearVertexAttributes[0].regIndex = sceGxmProgramParameterGetResourceIndex( paramClearPositionAttribute );
	clearVertexStreams[0].stride = sizeof(ClearVertex);
	clearVertexStreams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

	/* create clear programs */
	returnCode = sceGxmShaderPatcherCreateVertexProgram( s_shaderPatcher, s_clearVertexProgramId, clearVertexAttributes, 1, clearVertexStreams, 1, &s_clearVertexProgram );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

	returnCode = sceGxmShaderPatcherCreateFragmentProgram( s_shaderPatcher, s_clearFragmentProgramId,
														   SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4, SCE_GXM_MULTISAMPLE_NONE, NULL,
														   sceGxmShaderPatcherGetProgramFromId(s_clearVertexProgramId), &s_clearFragmentProgram );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

	/* create the clear triangle vertex/index data */
	s_clearVertices = (ClearVertex *)graphicsAlloc( SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE, 3*sizeof(ClearVertex), 4, SCE_GXM_MEMORY_ATTRIB_READ, &s_clearVerticesUId );
	s_clearIndices = (uint16_t *)graphicsAlloc( SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE, 3*sizeof(uint16_t), 2, SCE_GXM_MEMORY_ATTRIB_READ, &s_clearIndicesUId );

	s_clearVertices[0].x = -1.0f;
	s_clearVertices[0].y = -1.0f;
	s_clearVertices[1].x =  3.0f;
	s_clearVertices[1].y = -1.0f;
	s_clearVertices[2].x = -1.0f;
	s_clearVertices[2].y =  3.0f;

	s_clearIndices[0] = 0;
	s_clearIndices[1] = 1;
	s_clearIndices[2] = 2;

    // !! All related to triangle.

    /* get attributes by name to create vertex format bindings */
	/* first retrieve the underlying program to extract binding information */
	const SceGxmProgram *basicProgram = sceGxmShaderPatcherGetProgramFromId( s_basicVertexProgramId );
	SCE_DBG_ALWAYS_ASSERT( basicProgram );
	const SceGxmProgramParameter *paramBasicPositionAttribute = sceGxmProgramFindParameterByName( basicProgram, "aPosition" );
	SCE_DBG_ALWAYS_ASSERT( paramBasicPositionAttribute && ( sceGxmProgramParameterGetCategory(paramBasicPositionAttribute) == SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE ) );
	const SceGxmProgramParameter *paramBasicNormalAttribute = sceGxmProgramFindParameterByName( basicProgram, "aNormal" );
	SCE_DBG_ALWAYS_ASSERT( paramBasicNormalAttribute && ( sceGxmProgramParameterGetCategory(paramBasicNormalAttribute) == SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE ) );
	const SceGxmProgramParameter *paramBasicColorAttribute = sceGxmProgramFindParameterByName( basicProgram, "aColor" );
	SCE_DBG_ALWAYS_ASSERT( paramBasicColorAttribute && ( sceGxmProgramParameterGetCategory(paramBasicColorAttribute) == SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE ) );

	/* create shaded triangle vertex format */
	SceGxmVertexAttribute basicVertexAttributes[3];
	SceGxmVertexStream basicVertexStreams[1];
	basicVertexAttributes[0].streamIndex = 0;
	basicVertexAttributes[0].offset = 0;
	basicVertexAttributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	basicVertexAttributes[0].componentCount = 3;
	basicVertexAttributes[0].regIndex = sceGxmProgramParameterGetResourceIndex( paramBasicPositionAttribute );
	basicVertexAttributes[1].streamIndex = 0;
	basicVertexAttributes[1].offset = 12;
	basicVertexAttributes[1].format = SCE_GXM_ATTRIBUTE_FORMAT_F32; // Mapping relation clarified.
	basicVertexAttributes[1].componentCount = 3;
	basicVertexAttributes[1].regIndex = sceGxmProgramParameterGetResourceIndex( paramBasicNormalAttribute );
	basicVertexAttributes[2].streamIndex = 0;
	basicVertexAttributes[2].offset = 24;
	basicVertexAttributes[2].format = SCE_GXM_ATTRIBUTE_FORMAT_U8N;
	basicVertexAttributes[2].componentCount = 4;
	basicVertexAttributes[2].regIndex = sceGxmProgramParameterGetResourceIndex(  paramBasicColorAttribute);
	basicVertexStreams[0].stride = sizeof(BasicVertex);
	basicVertexStreams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

	/* create shaded triangle shaders */
	returnCode = sceGxmShaderPatcherCreateVertexProgram( s_shaderPatcher, s_basicVertexProgramId, basicVertexAttributes, 3,
														 basicVertexStreams, 1, &s_basicVertexProgram );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

	returnCode = sceGxmShaderPatcherCreateFragmentProgram( s_shaderPatcher, s_basicFragmentProgramId,
														   SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4, SCE_GXM_MULTISAMPLE_NONE, NULL,
														   sceGxmShaderPatcherGetProgramFromId(s_basicVertexProgramId), &s_basicFragmentProgram );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

	/* find vertex uniforms by name and cache parameter information */
	s_wvpParam = sceGxmProgramFindParameterByName( basicProgram, "wvp" );
	s_wParam = sceGxmProgramFindParameterByName( basicProgram, "w" );
	SCE_DBG_ALWAYS_ASSERT( s_wvpParam && ( sceGxmProgramParameterGetCategory( s_wvpParam ) == SCE_GXM_PARAMETER_CATEGORY_UNIFORM ) );
	SCE_DBG_ALWAYS_ASSERT( s_wParam && ( sceGxmProgramParameterGetCategory( s_wParam ) == SCE_GXM_PARAMETER_CATEGORY_UNIFORM ) );

	/* create shaded triangle vertex/index data 
	s_basicVertices = (BasicVertex *)graphicsAlloc( SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE, 4 * 6 * 27 * sizeof(BasicVertex), 4, SCE_GXM_MEMORY_ATTRIB_READ, &s_basicVerticesUId );
	s_basicIndices = (uint16_t *)graphicsAlloc( SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE, 6 * 6 * 27 * sizeof(uint16_t), 2, SCE_GXM_MEMORY_ATTRIB_READ, &s_basicIndiceUId );*/
}




/* Main render function */
void render( void )
{
	/* render libgxm scenes */
	renderGxm();
}

/* render gxm scenes */
void renderGxm( void )
{
/* -----------------------------------------------------------------
	8. Rendering step

	This sample renders a single scene containing the clear triangle.
	Before any drawing can take place, a scene must be started.
	We render to the back buffer, so it is also important to use a
	sync object to ensure that these rendering operations are
	synchronized with display operations.

	The clear triangle shaders do not declare any uniform variables,
	so this may be rendered immediately after setting the vertex and
	fragment program.

	Once clear triangle have been drawn the scene can be ended, which
	submits it for rendering on the GPU.
	----------------------------------------------------------------- */

	/* start rendering to the render target */
	sceGxmBeginScene( s_context, 0, s_renderTarget, NULL, NULL, s_displayBufferSync[s_displayBackBufferIndex], &s_displaySurface[s_displayBackBufferIndex], &s_depthSurface );

	/* set clear shaders */
	sceGxmSetVertexProgram( s_context, s_clearVertexProgram );
	sceGxmSetFragmentProgram( s_context, s_clearFragmentProgram );

	/* draw ther clear triangle */
	sceGxmSetVertexStream( s_context, 0, s_clearVertices );
	sceGxmDraw( s_context, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, s_clearIndices, 3 );


    // !! Speciality for rendering a triangle.

    /* render the  triangle */
	sceGxmSetVertexProgram( s_context, s_basicVertexProgram );
	sceGxmSetFragmentProgram( s_context, s_basicFragmentProgram );

	for (int i=0; i < NUM_CUBES; ++i)
	{
		cubeList[i]->DrawCube(s_context, s_View, s_Projection, s_globalRotation, s_wvpParam, s_wParam);
	}

	/* stop rendering to the render target */
	sceGxmEndScene( s_context, NULL, NULL );
}



/* queue a display swap and cycle our buffers */
void cycleDisplayBuffers( void )
{
/* -----------------------------------------------------------------
	9-a. Flip operation

	Now we have finished submitting rendering work for this frame it
	is time to submit a flip operation.  As part of specifying this
	flip operation we must provide the sync objects for both the old
	buffer and the new buffer.  This is to allow synchronization both
	ways: to not flip until rendering is complete, but also to ensure
	that future rendering to these buffers does not start until the
	flip operation is complete.

	Once we have queued our flip, we manually cycle through our back
	buffers before starting the next frame.
	----------------------------------------------------------------- */

	/* PA heartbeat to notify end of frame */
	sceGxmPadHeartbeat( &s_displaySurface[s_displayBackBufferIndex], s_displayBufferSync[s_displayBackBufferIndex] );

	/* queue the display swap for this frame */
	DisplayData displayData;
	displayData.address = s_displayBufferData[s_displayBackBufferIndex];

	/* front buffer is OLD buffer, back buffer is NEW buffer */
	sceGxmDisplayQueueAddEntry( s_displayBufferSync[s_displayFrontBufferIndex], s_displayBufferSync[s_displayBackBufferIndex], &displayData );

	/* update buffer indices */
	s_displayFrontBufferIndex = s_displayBackBufferIndex;
	s_displayBackBufferIndex = (s_displayBackBufferIndex + 1) % DISPLAY_BUFFER_COUNT;
}


/* Destroy Gxm Data */
void destroyGxmData( void )
{
/* ---------------------------------------------------------------------
	11. Destroy the programs and data for the clear and spinning triangle

	Once the GPU is finished, we release all our programs.
	--------------------------------------------------------------------- */

	/* clean up allocations */
	sceGxmShaderPatcherReleaseFragmentProgram( s_shaderPatcher, s_clearFragmentProgram );
	sceGxmShaderPatcherReleaseVertexProgram( s_shaderPatcher, s_clearVertexProgram );
	graphicsFree( s_clearIndicesUId );
	graphicsFree( s_clearVerticesUId );

	/* wait until display queue is finished before deallocating display buffers */
	sceGxmDisplayQueueFinish();

	/* unregister programs and destroy shader patcher */
	sceGxmShaderPatcherUnregisterProgram( s_shaderPatcher, s_clearFragmentProgramId );
	sceGxmShaderPatcherUnregisterProgram( s_shaderPatcher, s_clearVertexProgramId );
	sceGxmShaderPatcherDestroy( s_shaderPatcher );
	fragmentUsseFree( s_patcherFragmentUsseUId );
	vertexUsseFree( s_patcherVertexUsseUId );
	graphicsFree( s_patcherBufferUId );
}



/* ShutDown libgxm */
int shutdownGxm( void )
{
/* ---------------------------------------------------------------------
	12. Finalize libgxm

	Once the GPU is finished, we deallocate all our memory,
	destroy all object and finally terminate libgxm.
	--------------------------------------------------------------------- */

	int returnCode = SCE_OK;

	graphicsFree( s_depthBufferUId );

	for ( unsigned int i = 0 ; i < DISPLAY_BUFFER_COUNT; ++i )
	{
		memset( s_displayBufferData[i], 0, DISPLAY_HEIGHT*DISPLAY_STRIDE_IN_PIXELS*4 );
		graphicsFree( s_displayBufferUId[i] );
		sceGxmSyncObjectDestroy( s_displayBufferSync[i] );
	}

	/* destroy the render target */
	sceGxmDestroyRenderTarget( s_renderTarget );

	/* destroy the context */
	sceGxmDestroyContext( s_context );

	fragmentUsseFree( s_fragmentUsseRingBufferUId );
	graphicsFree( s_fragmentRingBufferUId );
	graphicsFree( s_vertexRingBufferUId );
	graphicsFree( s_vdmRingBufferUId );
	free( s_contextParams.hostMem );

	/* terminate libgxm */
	sceGxmTerminate();

	return returnCode;
}


/* Host alloc */
static void *patcherHostAlloc( void *userData, unsigned int size )
{
	(void)( userData );

	return malloc( size );
}

/* Host free */
static void patcherHostFree( void *userData, void *mem )
{
	(void)( userData );

	free( mem );
}

/* Display callback */
void displayCallback( const void *callbackData )
{
/* -----------------------------------------------------------------
	10-b. Flip operation

	The callback function will be called from an internal thread once
	queued GPU operations involving the sync objects is complete.
	Assuming we have not reached our maximum number of queued frames,
	this function returns immediately.
	----------------------------------------------------------------- */

	SceDisplayFrameBuf framebuf;

	/* cast the parameters back */
	const DisplayData *displayData = (const DisplayData *)callbackData;


    // Render debug text.
    /* set framebuffer info */
	SceDbgFontFrameBufInfo info;
	memset( &info, 0, sizeof(SceDbgFontFrameBufInfo) );
	info.frameBufAddr = (SceUChar8 *)displayData->address;
	info.frameBufPitch = DISPLAY_STRIDE_IN_PIXELS;
	info.frameBufWidth = DISPLAY_WIDTH;
	info.frameBufHeight = DISPLAY_HEIGHT;
	info.frameBufPixelformat = DBGFONT_PIXEL_FORMAT;

	/* flush font buffer */
	int returnCode = sceDbgFontFlush( &info );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );
	

	/* wwap to the new buffer on the next VSYNC */
	memset(&framebuf, 0x00, sizeof(SceDisplayFrameBuf));
	framebuf.size        = sizeof(SceDisplayFrameBuf);
	framebuf.base        = displayData->address;
	framebuf.pitch       = DISPLAY_STRIDE_IN_PIXELS;
	framebuf.pixelformat = DISPLAY_PIXEL_FORMAT;
	framebuf.width       = DISPLAY_WIDTH;
	framebuf.height      = DISPLAY_HEIGHT;
	returnCode = sceDisplaySetFrameBuf( &framebuf, SCE_DISPLAY_UPDATETIMING_NEXTVSYNC );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

	/* block this callback until the swap has occurred and the old buffer is no longer displayed */
	returnCode = sceDisplayWaitVblankStart();
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );
}








