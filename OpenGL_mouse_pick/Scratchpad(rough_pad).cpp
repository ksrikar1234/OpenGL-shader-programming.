outColor = vec3( float(TriangleID & 0xff)/255.0 , float((TriangleID  >> 8) & 0xff)/255.0 ,  float((TriangleID  >> 16) & 0xff)/255.0 );

outColor = vec3(TriangleID/459706,TriangleID/459706,TriangleID/459706);


//glm::mat4 view = glm::translate(glm::mat4(0.1f), glm::vec3((translate_mouse_x*2/SCR_WIDTH)-1.0f, (1.0f-(2.0f*translate_mouse_y/SCR_HEIGHT)), -3.0f));


196 130 0 
0.768627 0.509804 0