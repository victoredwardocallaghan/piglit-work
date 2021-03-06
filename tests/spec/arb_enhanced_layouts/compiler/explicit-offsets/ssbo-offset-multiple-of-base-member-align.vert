// [config]
// expect_result: fail
// glsl_version: 1.40
// require_extensions: GL_ARB_enhanced_layouts GL_ARB_shader_storage_buffer_object
// check_link: false
// [end config]
//
// ARB_enhanced_layouts spec says:
//    "The specified offset must be a
//    multiple of the base alignment of the type of the block member it
//    qualifies, or a compile-time error results."
//
// Tests for successful compilation, when the block is of std140 layout.
//

#version 140
#extension GL_ARB_enhanced_layouts : enable
#extension GL_ARB_shader_storage_buffer_object : enable

layout(std430) buffer b {
       layout(offset = 0) float f1;
       layout(offset = 6) float f2; // Wrong: offset must be aligned to multiple of sizeof(float)
};

void main()
{
}
