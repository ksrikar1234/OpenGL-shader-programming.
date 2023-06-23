#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
class memory {
public:
std::vector<int> x;
std::vector<int> y;
std::vector<int> z;
std::vector<std::string> s;

};


inline std::vector<unsigned long int > MemoryUsage(std::string message = "" , std::vector<unsigned long int> prev_mem_status  = {0,0} , bool print_message = true )
{
    std::ifstream statusFile("/proc/self/status");
    std::string line;
    std::vector<unsigned long > memory_usage;
    memory_usage.reserve(3);    
    std::cout << message ;
    
    bool memory_change_flag = false;
    
    while (std::getline(statusFile, line))
    {
        std::istringstream iss(line);
        std::string key;
        unsigned long value;
        
        if (iss >> key >> value)
        {
            if (key == "VmSize:")
            {
                // Virtual memory size in kilobytes
               
                 if(value - prev_mem_status[0] > 0 )
                 {
                   if(print_message)
                      {
                        std::cout << "Current Virtual Memory Usage : " << value  << " KB" << '\n';
                        std::cout << " -----------------> Extra Virtual Memory Used: " << value - prev_mem_status[0] << " KB" << '\n';  
                      }
                          
                   memory_change_flag = true;
                 }                 
                   memory_usage.push_back(value);                
            }               
            else if (key == "VmRSS:")
            {
                // Resident set size (physical memory) in kilobytes
              
                if( value - prev_mem_status[1] > 0 )
                {    
                   if(print_message)
                   {
                     std::cout << " Current Physical Memory Used: " << value << " KB" << '\n';            
                     std::cout << " ----------------> Extra Physical Memory Used: " << value - prev_mem_status[1] << " KB" << '\n';
                   } 
                   memory_change_flag = true;
                }   
                   memory_usage.push_back(value);
              
            }
        
         memory_change_flag==true ? memory_usage.push_back(10) :  memory_usage.push_back(0);
        
        }
         
      }

return memory_usage;

}




int main()
{
  auto mem_track = MemoryUsage();  
  memory object1;
  mem_track = MemoryUsage("After declaring object 1 \n" , mem_track );
  memory object2;
  mem_track = MemoryUsage("After Declaring object 2 \n " , mem_track );
  
//  object1.x.reserve(10000); // use reserve for controlling preallocation
//  object2.x.reserve(10000);
  for(int i = 0 ; i < 100000; i++)
    {

        object1.x.push_back(20);
         
        if(mem_track[2] > 0) { std::cout << "Object2 Loop at " << i << "allocated more memory \n"; mem_track = MemoryUsage( "" , mem_track , true ); }

        mem_track = MemoryUsage( "" , mem_track , false );
    } 
  
   for(int i = 0 ; i < 100000; i++)
    {
        object2.x.push_back(20); 
        
        if(mem_track[2] > 0) { std::cout << "Object2 Loop at " << i << "allocated more memory \n"; mem_track = MemoryUsage( "" , mem_track , true ); }
        
        mem_track = MemoryUsage( "" , mem_track , false );       
    }  
  
}

