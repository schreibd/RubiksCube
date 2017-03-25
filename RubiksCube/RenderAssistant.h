#pragma once

#ifndef RENDER_ASSISTANT_H_
#define	RENDER_ASSISTANT_H_

#include <kernel.h>
#include <vectormath.h>
#include <libdbg.h>
#include <stdlib.h>
#include <stdio.h>
#include <gxm/memory.h>
#include <gxm.h>


#define ALIGN(x, a)					(((x) + ((a) - 1)) & ~((a) - 1))

 void *graphicsAlloc( SceKernelMemBlockType type, uint32_t size, uint32_t alignment, uint32_t attribs, SceUID *uid );
 /*	Helper function to free memory mapped to the GPU */
 void graphicsFree( SceUID uid );
 /* Helper function to allocate memory and map it as vertex USSE code for the GPU */
 void *vertexUsseAlloc( uint32_t size, SceUID *uid, uint32_t *usseOffset );
 /* Helper function to free memory mapped as vertex USSE code for the GPU */
 void vertexUsseFree( SceUID uid );
 /* Helper function to allocate memory and map it as fragment USSE code for the GPU */
 void *fragmentUsseAlloc( uint32_t size, SceUID *uid, uint32_t *usseOffset );
 /* Helper function to free memory mapped as fragment USSE code for the GPU */
 void fragmentUsseFree( SceUID uid );

#endif