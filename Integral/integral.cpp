// -----------------------------------------------------------------------------
// Sistemas concurrentes y Distribuidos.
// Seminario 1. Programación Multihebra y Semáforos.
//
// Calculo concurrente de una integral.
// Calculándola además de dos formas distintas: entrelazada y contigua.
//
// Autor: Francisco Domínguez
// -----------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <chrono>  // incluye now, time\_point, duration
#include <future>
#include <cmath>

using namespace std;
using namespace std::chrono;

const long m = 1024l*1024l*1024l, n = 4;


// -----------------------------------------------------------------------------
// evalua la función $f$ a integrar ($f(x)=4/(1+x^2)$)
double f(double x){
  return 4.0/(1.0+x*x);
}
// -----------------------------------------------------------------------------
// calcula la integral de forma secuencial, devuelve resultado:
double calcular_integral_secuencial(){
   double suma = 0.0 ;                        // inicializar suma
   for(long i = 0 ; i<m ; i++)            // para cada $i$ entre $0$ y $m-1$:
      suma += f((i+double(0.5))/m);         //   $~$ añadir $f(x_i)$ a la suma actual
   return suma/m ;                            // devolver valor promedio de $f$
}

// -----------------------------------------------------------------------------
// función que ejecuta cada hebra: recibe $i$ ==índice de la hebra, ($0\leq i<n$)
double funcion_hebra(long ih){
   double suma_parcial = 0.0;
  
   for(long i=(ih*m/n); i<(ih*m/n+m/n); i++){
       suma_parcial += f((i+double(0.5))/m);
   }

   return suma_parcial;
}

// -----------------------------------------------------------------------------
// calculo de la integral de forma concurrente
double calcular_integral_concurrente( )
{
  double resultado = 0.0;
  future<double> futuros[n];

  for(long i=0; i<n; i++){
    futuros[i] = async(launch::async, funcion_hebra, i);
  }

  for(long i=0; i<n; i++){
    resultado += futuros[i].get();
  }

  return resultado/m;

}
// -----------------------------------------------------------------------------

int main(){

  time_point<steady_clock> inicio_sec  = steady_clock::now();
  const double             result_sec  = calcular_integral_secuencial();
  time_point<steady_clock> fin_sec     = steady_clock::now();
  double x = sin(0.4567);
  time_point<steady_clock> inicio_conc = steady_clock::now();
  const double             result_conc = calcular_integral_concurrente();
  time_point<steady_clock> fin_conc    = steady_clock::now();
  duration<float,milli>    tiempo_sec  = fin_sec  - inicio_sec,
                           tiempo_conc = fin_conc - inicio_conc;
  const float              porc        = 100.0*tiempo_conc.count()/tiempo_sec.count();


  constexpr double pi = 3.14159265358979323846l;

  cout << "Número de muestras (m)   : " << m << endl
       << "Número de hebras (n)     : " << n << endl
       << setprecision(18)
       << "Valor de PI              : " << pi << endl
       << "Resultado secuencial     : " << result_sec  << endl
       << "Resultado concurrente    : " << result_conc << endl
       << setprecision(5)
       << "Tiempo secuencial        : " << tiempo_sec.count()  << " milisegundos. " << endl
       << "Tiempo concurrente       : " << tiempo_conc.count() << " milisegundos. " << endl
       << setprecision(4)
       << "Porcentaje t.conc/t.sec. : " << porc << "%" << endl;
}
