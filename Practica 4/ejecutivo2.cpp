/*
 Sistemas concurrentes y Distribuidos.
 Práctica 4. Implementación de sistemas de tiempo real.

 Archivo: ejecutivo2.cpp
 Implementación del segundo ejemplo de ejecutivo cíclico:
 autor: Francisco Domínguez Lorente

   Datos de las tareas:
   ------------
   Ta.  T    C
   ------------
   A  500   100
   B  500   150
   C  1000  200
   D  2000  240
  -------------

  Planificación (con Ts == 500 ms)
  *---------*--------*---------*------*
  | A B C   | A B D  | A B C   | A B  |
  *---------*--------*---------*------*
*/

#include <string>
#include <iostream> // cout, cerr
#include <thread>
#include <chrono>   // utilidades de tiempo
#include <ratio>    // std::ratio_divide

using namespace std ;
using namespace std::chrono ;
using namespace std::this_thread ;

// tipo para duraciones en segundos y milisegundos, en coma flotante:
typedef duration<float,ratio<1,1>>    seconds_f ;
typedef duration<float,ratio<1,1000>> milliseconds_f ;

// -----------------------------------------------------------------------------
// tarea genérica: duerme durante un intervalo de tiempo (de determinada duración)

void Tarea( const std::string & nombre, milliseconds tcomputo )
{
   cout << "   Comienza tarea " << nombre << " (C == " << tcomputo.count() << " ms.) ... " ;
   sleep_for( tcomputo );
   cout << "fin." << endl ;
}

// -----------------------------------------------------------------------------
// tareas concretas del problema:

void TareaA() { Tarea( "A", milliseconds( 100) );  }
void TareaB() { Tarea( "B", milliseconds( 150) );  }
void TareaC() { Tarea( "C", milliseconds( 200) );  }
void TareaD() { Tarea( "D", milliseconds( 240) );  }

// -----------------------------------------------------------------------------
// implementación del ejecutivo cíclico:

int main( int argc, char *argv[] )
{
   // Ts = duración del ciclo secundario
   const milliseconds Ts( 500 );

   // ini_sec = instante de inicio de la iteración actual del ciclo secundario
   time_point<steady_clock> ini_sec = steady_clock::now();

   while( true ) // ciclo principal
   {
      cout << endl
           << "---------------------------------------" << endl
           << "Comienza iteración del ciclo principal." << endl ;

      for( int i = 1 ; i <= 4 ; i++ ) // ciclo secundario (4 iteraciones)
      {
         cout << endl << "Comienza iteración " << i << " del ciclo secundario." << endl ;
         milliseconds sobrantes = milliseconds(0);
         switch( i )
         {
            case 1 : TareaA(); TareaB(); TareaC();
            sobrantes  = Ts-milliseconds(100)-milliseconds(150)-milliseconds(200);          break ;
            case 2 : TareaA(); TareaB(); TareaD();
            sobrantes  = Ts-milliseconds(100)-milliseconds(150)-milliseconds(240);           break ;
            case 3 : TareaA(); TareaB(); TareaC(); 
            sobrantes  = Ts-milliseconds(100)-milliseconds(150)-milliseconds(200);          break ;
            case 4 : TareaA(); TareaB();
            sobrantes = Ts-milliseconds(100)-milliseconds(150);                     break ;
         }

         // calcular el siguiente instante de inicio del ciclo secundario
         ini_sec += Ts ;

         // esperar hasta el inicio de la siguiente iteración del ciclo secundario
         sleep_until( ini_sec );

         time_point<steady_clock> ini_fin = steady_clock::now();
         steady_clock::duration duracion = ini_fin - ini_sec;
         cout << endl << "La deriva es " << milliseconds_f(duracion).count() << " milisegundos" << endl;
         cout << "El espacio sobrante en el ciclo " << i << " es " << sobrantes.count() << " milisegundos" << endl;
      }
   }
}
