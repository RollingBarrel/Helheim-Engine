#ifndef FREE_NEHE_H
#define FREE_NEHE_H

// FreeType Headers
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

// OpenGL Headers
#include <windows.h>                                      // (The GL Headers Need It)
#include <GL/gl.h>
#include <GL/glu.h>

// Some STL Headers
#include <vector>
#include <string>

// Using The STL Exception Library Increases The
// Chances That Someone Else Using Our Code Will Correctly
// Catch Any Exceptions That We Throw.
#include <stdexcept>

// MSVC Will Spit Out All Sorts Of Useless Warnings If
// You Create Vectors Of Strings, This Pragma Gets Rid Of Them.
#pragma warning(disable: 4786)

// Wrap Everything In A Namespace, That Way We Can Use A Common
// Function Name Like "print" Without Worrying About
// Overlapping With Anyone Else's Code.
namespace freetype 
{

    // Inside Of This Namespace, Give Ourselves The Ability
    // To Write Just "vector" Instead Of "std::vector"
    using std::vector;

    // Ditto For String.
    using std::string;

    // This Holds All Of The Information Related To Any
    // FreeType Font That We Want To Create. 
    struct font_data {
        float h;                                        // Holds The Height Of The Font.
        GLuint* textures;                                  // Holds The Texture Id's
        GLuint list_base;                                   // Holds The First Display List Id

        // The Init Function Will Create A Font With
        // The Height h From The File fname.
        void init(const char* fname, unsigned int h);

        // Free All The Resources Associated With The Font.
        void clean();
    };

    // The Flagship Function Of The Library - This Thing Will Print
    // Out Text At Window Coordinates X, Y, Using The Font ft_font.
    // The Current Modelview Matrix Will Also Be Applied To The Text.
    void print(const font_data& ft_font, float x, float y, const char* fmt, ...);

}                                               // Close The Namespace

#endif