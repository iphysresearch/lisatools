#include <stdio.h>
#include <stdlib.h>

#include "io-C/lisaxml.h"

int main(int argc,char **argv) {
    writeXML *myxml;
    char buffer[256];
    char tag[100];
    char filename[100];
    double A, f, theta, phi, iota, psi, iphase;
    FILE* input;

    input = fopen("gb.txt","r");

  while (!feof(input))
     {
      fscanf(input, "%s %lg %lg %lg %lg %lg %lg %lg\n",    
      &tag, &theta, &phi, &psi, &A, &iota, &f, &iphase);
     

    sprintf(filename, "XML/%s.xml", tag);
    
    myxml = XMLopen(filename);

    /* Standard beginning of the file */

    XMLcontentstring(myxml,"<?xml version=\"1.0\"?>");
    XMLcontentstring(myxml,"<!DOCTYPE XSIL SYSTEM \"http://www.vallis.org/lisa-xml.dtd\">");

    XMLcontentstring(myxml,"<?xml-stylesheet type=\"text/xsl\" href=\"http://www.vallis.org/lisa-xml.xsl\"?>");

    XMLopentag(myxml,"XSIL","");

        /* Prolog */

        XMLopentag(myxml,"Param","Name=\"Author\"");
            XMLcontentstring(myxml,"Neil Cornish");
        XMLclosetag(myxml,"Param");
    
        XMLopentag(myxml,"Param","Name=\"GenerationDate\" Type=\"ISO-8601\"");
            getISOtime(buffer);
            XMLcontentstring(myxml,buffer);
        XMLclosetag(myxml,"Param");

        XMLopentag(myxml,"Comment","");
            XMLcontentstring(myxml,"This file generated by galactic_xml.c (NJC 2006/06/06)");
        XMLclosetag(myxml,"Comment");

        /* SourceData section */

        XMLopentag(myxml,"XSIL","Type=\"SourceData\"");

            XMLopentag(myxml,"XSIL","Name=\"%s\" Type=\"PlaneWave\"",tag);

                /* Give parameters as needed */

                XMLparamdouble(myxml,"EclipticLatitude","Radian",theta);
                XMLparamdouble(myxml,"EclipticLongitude","Radian",phi);
                XMLparamdouble(myxml,"Polarization","Radian",psi);

                XMLparamstring(myxml,"SourceType","String","GalacticBinary");

                XMLparamdouble(myxml,"Amplitude","1",A);

                XMLparamdouble(myxml,"Inclination","Radian",iota);
                XMLparamdouble(myxml,"Frequency","Hertz",f);
                XMLparamdouble(myxml,"InitialPhase","Radian",iphase);  

                /* Not needed in source file, but only in hp/hc file */ 
                /* XMLparamdouble(myxml,"TimeOffset","Second",900.0); */

            XMLclosetag(myxml,"XSIL");
            
        XMLclosetag(myxml,"XSIL");

    XMLclosetag(myxml,"XSIL");

    XMLclose(myxml);

     } /* end loop over sources */

}
