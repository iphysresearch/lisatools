#include <stdio.h>
#include <stdlib.h>
#include "lisaxml.h"

#define XMLSTDINDENT 4

void KILL(char*);

int main(int argc,char **argv) {
    writeXML *myxml;
    char buffer[256];
    char tag[100];
    char galaxy[100];
    char bright[100];
    char filename[100];
    char ErrorMessage[100];
    long cnt, cntb;
    double A, f, theta, phi, iota, psi, iphase;
    FILE* input;

  if (argc != 3)
  {
    printf("The correct call of this program is:\n\n");
    printf("   ./Galaxy_key name seed\n");
    KILL("Please try again.\n");
  }     

    sprintf(tag, "Galaxy_%s", argv[2]);
    sprintf(galaxy, "Data/Galaxy_%s.dat", argv[2]);
    sprintf(bright, "Data/Galaxy_Bright_%s.dat", argv[2]);

    sprintf(filename, "Data/count_%s.dat", argv[2]);
    input = fopen(filename,"r");
    fscanf(input,"%ld%ld\n", &cnt, &cntb);
    fclose(input);

    sprintf(filename, "XML/%s_key.xml", argv[1]);

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
            XMLcontentstring(myxml,"This file generated by Galaxy_key.c (NJC 2006/11/02)");
        XMLclosetag(myxml,"Comment");

        /* SourceData section */

        XMLopentag(myxml,"XSIL","Type=\"SourceData\"");

        /* Begin first table */

            XMLopentag(myxml,"XSIL","Name=\"%s-bright\" Type=\"PlaneWaveTable\"",tag);

                XMLopentag(myxml,"Param", "Name=\"SourceType\" Unit=\"String\"");
                    XMLcontentstring(myxml,"GalacticBinary");
                XMLclosetag(myxml,"Param");

                XMLopentag(myxml,"Table","");

                    XMLopentag(myxml,"Column", "Name=\"Frequency\" Type=\"double\" Unit=\"Hertz\"/");
                    myxml->indent -= XMLSTDINDENT;
                    XMLopentag(myxml,"Column", "Name=\"EclipticLatitude\" Type=\"double\" Unit=\"Radian\"/");
                    myxml->indent -= XMLSTDINDENT;
                    XMLopentag(myxml,"Column", "Name=\"EclipticLongitude\" Type=\"double\" Unit=\"Radian\"/");
                    myxml->indent -= XMLSTDINDENT;
                    XMLopentag(myxml,"Column", "Name=\"Amplitude\" Type=\"double\" Unit=\"1\"/");
                    myxml->indent -= XMLSTDINDENT;
                    XMLopentag(myxml,"Column", "Name=\"Inclination\" Type=\"double\" Unit=\"Radian\"/");
                    myxml->indent -= XMLSTDINDENT;
                    XMLopentag(myxml,"Column", "Name=\"Polarization\" Type=\"double\" Unit=\"Radian\"/");
                    myxml->indent -= XMLSTDINDENT;
                    XMLopentag(myxml,"Column", "Name=\"InitialPhase\" Type=\"double\" Unit=\"Radian\"/");

                    XMLdimlong(myxml,"Length",cntb);
                    XMLdimlong(myxml,"Records",7);

                    XMLopentag(myxml,"Stream","Type=\"Remote\" Encoding=\"Text\"");
                        XMLcontentstring(myxml,bright);
                    XMLclosetag(myxml,"Stream");

                myxml->indent -= XMLSTDINDENT;
                XMLclosetag(myxml,"Table");

            XMLclosetag(myxml,"XSIL");

        /* Begin second table */

            XMLopentag(myxml,"XSIL","Name=\"%s\" Type=\"PlaneWaveTable\"",tag);

                XMLopentag(myxml,"Param", "Name=\"SourceType\" Unit=\"String\"");
                    XMLcontentstring(myxml,"GalacticBinary");
                XMLclosetag(myxml,"Param");

            XMLopentag(myxml,"Table","");

                XMLopentag(myxml,"Column", "Name=\"Frequency\" Type=\"double\" Unit=\"Hertz\"/");
                myxml->indent -= XMLSTDINDENT;
                XMLopentag(myxml,"Column", "Name=\"EclipticLatitude\" Type=\"double\" Unit=\"Radian\"/");
                myxml->indent -= XMLSTDINDENT;
                XMLopentag(myxml,"Column", "Name=\"EclipticLongitude\" Type=\"double\" Unit=\"Radian\"/");
                myxml->indent -= XMLSTDINDENT;
                XMLopentag(myxml,"Column", "Name=\"Amplitude\" Type=\"double\" Unit=\"1\"/");
                myxml->indent -= XMLSTDINDENT;
                XMLopentag(myxml,"Column", "Name=\"Inclination\" Type=\"double\" Unit=\"Radian\"/");
                myxml->indent -= XMLSTDINDENT;
                XMLopentag(myxml,"Column", "Name=\"Polarization\" Type=\"double\" Unit=\"Radian\"/");
                myxml->indent -= XMLSTDINDENT;
                XMLopentag(myxml,"Column", "Name=\"InitialPhase\" Type=\"double\" Unit=\"Radian\"/");

                XMLdimlong(myxml,"Length",cnt);
                XMLdimlong(myxml,"Records",7);

                XMLopentag(myxml,"Stream","Type=\"Remote\" Encoding=\"Text\"");
                    XMLcontentstring(myxml,galaxy);
                XMLclosetag(myxml,"Stream");

            myxml->indent -= XMLSTDINDENT;
            XMLclosetag(myxml,"Table");

        XMLclosetag(myxml,"XSIL");

        /* End second table */
            
        XMLclosetag(myxml,"XSIL");

    XMLclosetag(myxml,"XSIL");

    XMLclose(myxml);

    exit(0);
}

void KILL(char* Message)
{
  printf("\a\n");
  printf(Message);
  printf("Terminating the program.\n\n\n");
  exit(1);
 
  return;
}
