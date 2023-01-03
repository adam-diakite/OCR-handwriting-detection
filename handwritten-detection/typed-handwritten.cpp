#include <stdio.h>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <math.h> 
#include <vector>
using namespace cv;
using namespace std;


//Effectuer un seuillage en fonction des paramètres à disposition 
//n = niveau du seuil, aide = résultat du seuil, image = image initiale
void seuillage(int n, Mat image, Mat aide) {

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (image.at<Vec3b>(i, j)[0] > n) {
                // question 1
                aide.at<uchar>(i, j) = 255;
            }

        }
    }

}


//Produire un histogramme des niveaux de gris
//hist = contient les valeurs et taille définit la taille de l'histo
void histogramme(Mat image, int hist[], int taille) {

    for (int i = 0; i < taille; i++) {
        hist[i] = 0;
    }

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            hist[image.at<Vec3b>(i, j)[0]]++;
        }


    }


}




//Affiche un tableau d'entiers
//tab : le tableau  qu'on veut afficher 
//taille : la taille du tableau
void afficheTab(int tab[], int taille) {

    for (int i = 0; i < taille; i++) {
        cout << " tab " << i << " : " << tab[i] << endl;

    }

}


//Pareil que le tableau d'entier mais en double
void afficheTab(double tab[], int taille) {

    for (int i = 0; i < taille; i++) {
        //cout << " tab "<< i <<" : " <<tab[i]<<endl;
        cout << tab[i] << endl;
    }

}

// Faire la somme des valeurs d'un tableau 
// tab : un tableau de valeurs entières 
//taille : la taille du tableau 
int somme(int hist[], int taille) {
    int res = 0;
    for (int i = 0; i < taille; i++) {
        res += hist[i];
    }
    return res;
}

//Trouve le maximum du tableau et retourne l'indice
//liste : la liste où on veut trouver le maximum
// taille : la taille de la liste 
//retourne l'indice du maximum dans la liste 
int maximum(int liste[], int taille) {
    int max = liste[0];
    int indice = 0;

    for (int i = 0; i < taille; ++i) {
        if (liste[i] > max) {
            max = liste[i];
        }
    }

    return max;

}

Mat img;
Mat otsu;
Mat diff;
int threshval = 100;
int t = 100;
int nbConEachS[256][256];

int nbNewCC[256][256];


//avoir que les images qui ne sont pas dans ref et qui sont dans bw
//bw : image avec un changement de seuil 
//ref : image référence
//retourne une image de la différence de ref et bw
static Mat avoirDiff(Mat bw, Mat ref) {
    // clone l'image bw dans diff
    Mat diff = bw.clone();
    //avoir une image noire de diff
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            if (diff.at<uchar>(i, j) != 0) {

                diff.at<uchar>(i, j) = 0;


            }
        }
    }

    //avoir les pixels qui ne sont pas sur ref dans bw
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {

            if (ref.at<uchar>(i, j) == 0 && bw.at<uchar>(i, j) == 255) {
                diff.at<uchar>(i, j) = 255;
            }

        
        }
    }
    //imshow("diff",diff);
    //enlever les pixels qui sont isolés (1 CC = 1 pixel )
    for (int i = 1; i < img.rows - 1; i++) {
        for (int j = 1; j < img.cols - 1; j++) {
            //gauche-haut,milieu-haut,droit-haut,gauche-milieu,mileu-milieu,droit_milieu,gauche-bas, milieu-bas,droit-bas  

            int nbMemePixel = 0;
            for (int x = -1; x < 1; x++) {
                for (int y = -1; y < 1; y++) {
                    if (ref.at<uchar>(i + x, j + y) == 255 && bw.at<uchar>(i + x, j + y) == 255) {
                        nbMemePixel++;
                    }

                }
            }

            if (ref.at<uchar>(i, j) != 255 && bw.at<uchar>(i, j) == 255) {
                if (nbMemePixel != 0) {
                    diff.at<uchar>(i, j) = 0;
                }
            }



        }

    }
    return diff;

}

//afficher la fenêtre pour voir les modifications 

static void on_trackbar(int, void*) {

    Mat bw = threshval < 256 ? (img < threshval) : (img > threshval);
    Mat ref = t < 256 ? (img < t) : (img > t);

    imshow("seuillage", bw);


    //diff = avoirDiff(bw,ref);


    Mat labelImage(img.size(), CV_32S);

    //int nLabels = connectedComponents(diff, labelImage, 8);
    int nLabels = connectedComponents(bw, labelImage, 8);
    //nb de CC dans bw
    cout << nLabels << endl;
    //nbConEachS[t][threshval]=nLabels;
    std::vector<Vec3b> colors(nLabels);
    colors[0] = Vec3b(0, 0, 0);//background
    for (int label = 1; label < nLabels; ++label) {
        colors[label] = Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
    }
    Mat dst(img.size(), CV_8UC3);
    for (int r = 0; r < dst.rows; ++r) {
        for (int c = 0; c < dst.cols; ++c) {

            int label = labelImage.at<int>(r, c);
            Vec3b& pixel = dst.at<Vec3b>(r, c);
            pixel = colors[label];
        }
    }


    //Mettre les valeurs du label à 0
    int nb[nLabels];

    // voir si l'image de ref et l'image bw ont la même composante connexe au même endroit.
    for (int r = 0; r < dst.rows; ++r) {
        for (int c = 0; c < dst.cols; ++c) {

            if (ref.at<uchar>(r, c) == 255 && bw.at<uchar>(r, c) == 255) {
                int label = labelImage.at<int>(r, c);
                nb[label] = 0;
            }


        }
    }

    // Retirer les composantes connexes
   // Mat dst(img.size(), CV_8UC3);
    for (int r = 0; r < dst.rows; ++r) {
        for (int c = 0; c < dst.cols; ++c) {

            int label = labelImage.at<int>(r, c);
            if (nb[label] == 0) {
                Vec3b& pixel = dst.at<Vec3b>(r, c);
                pixel = colors[0];
            }

        }
    }


    //Ajoute la couleur blanche de ref dans dst
    for (int r = 0; r < dst.rows; ++r) {
        for (int c = 0; c < dst.cols; ++c) {

            if ((int)dst.at<Vec3b>(r, c)[0] == 0) {
                dst.at<Vec3b>(r, c)[0] = ref.at<uchar>(r, c);
                dst.at<Vec3b>(r, c)[1] = ref.at<uchar>(r, c);
                dst.at<Vec3b>(r, c)[2] = ref.at<uchar>(r, c);
            }

        }
    }


   

    imshow("Composantes connexes", dst);

}

//avoir l'image référence en fenêtre avec la methode on_trackbar()
static void myCallbackForT(int t, void*) {

    Mat ref = t < 256 ? (img < t) : (img > t);
    imshow("r�f�rence", ref);
    imshow("Composantes connexes", ref);
}


//Avoir les nouvelles CC
//fin : la fin de la recherche 
//debut : le début de la recherche
//pas : le nombre de pas pour arriver du début jusqu'à la fin
void avoirNCC(int fin, int debut, int pas) {

    cout << "fin  : " << fin << ", debut  : " << debut << endl;

    for (int t = debut; t < fin; t = t + pas) {

        int seuilPas = t + pas;
        /*************************Avoir les nouvelles composantes connexes à chaque itération dans l'image *********************************************************/

    // seuilPas ne dépasse pas les 255
        if (seuilPas > 256) {
            seuilPas = 255;

        }

        // avoir les image bw et ref 
        Mat bw = seuilPas < 256 ? (img < (seuilPas)) : (img > (seuilPas));
        Mat ref = t < 256 ? (img < t) : (img > t);

        imshow("Seuillage", bw);
        imshow("Ref", ref);


        //diff = avoirDiff(bw,ref);

       //Création  d'une image où on met les labels
        Mat labelImage(img.size(), CV_32S);

        //Le nombre de CC dans l'image bw
        int nLabels = connectedComponents(bw, labelImage, 8);

        // cout<<"nLables : "<<nLabels<<endl;

         //Fixer les couleurs pour chaque CC
        std::vector<Vec3b> colors(nLabels);
        colors[0] = Vec3b(0, 0, 0);//background
        for (int label = 1; label < nLabels; ++label) {
            colors[label] = Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
        }

        Mat dst(img.size(), CV_8UC3);
        for (int r = 0; r < dst.rows; ++r) {
            for (int c = 0; c < dst.cols; ++c) {

                int label = labelImage.at<int>(r, c);
                Vec3b& pixel = dst.at<Vec3b>(r, c);
                pixel = colors[label];
            }
        }


        //mettre le tableau nb les valeurs du label en 1 ,
        int nb[nLabels];
        for (int i = 0; i < nLabels; i++) {
            nb[i] = 1;
        }

        //on vérifie ici si l'image de ref et l'image bw ont la même composante connexe au même endroit, si oui on met 0 sinon rien 
        for (int r = 0; r < dst.rows; ++r) {
            for (int c = 0; c < dst.cols; ++c) {

                if (ref.at<uchar>(r, c) == 255 && bw.at<uchar>(r, c) == 255) {
                    int label = labelImage.at<int>(r, c);
                    nb[label] = 0;

                }


            }
        }

        //avoir le nombre de CC qui sont identiques dans ref et bw
        int n = 0;
        for (int i = 0; i < nLabels; i++) {
            if (nb[i] == 0) {
                n++;
            }
        }

        // Suppression des  composantes connexes qui sont dans ref ET dans bw, losrque nb[label]==0
       // Mat dst(img.size(), CV_8UC3);
        for (int r = 0; r < dst.rows; ++r) {
            for (int c = 0; c < dst.cols; ++c) {

                int label = labelImage.at<int>(r, c);
                if (nb[label] == 0) {
                    Vec3b& pixel = dst.at<Vec3b>(r, c);
                    pixel = colors[0];
                }

            }
        }


        //Rajoute la couleur blanc de ref dans dst
        for (int r = 0; r < dst.rows; ++r) {
            for (int c = 0; c < dst.cols; ++c) {

                if ((int)dst.at<Vec3b>(r, c)[0] == 0) {
                    dst.at<Vec3b>(r, c)[0] = ref.at<uchar>(r, c);
                    dst.at<Vec3b>(r, c)[1] = ref.at<uchar>(r, c);
                    dst.at<Vec3b>(r, c)[2] = ref.at<uchar>(r, c);
                }

            }
        }

        //Mettre les valeurs dans un tableau 
        nbNewCC[t][seuilPas] = nLabels - n;
        nbConEachS[t][seuilPas] = nLabels - (nLabels - n);
        // cout << "nbNewCC : "<<nbNewCC[t][t+pas]<<"\n"<<endl;





    }

}


//avoir les Composantes connexes dans seuillage et dans image 
//seuillage : une image seuillée de "image "
//image : une image binaire
// retourne une image qui contient les CC dans seuillage et image 
Mat haveCC(Mat seuillage, Mat image) {


    Mat labelImage(img.size(), CV_32S);

    // nombre de CC dans l'image seuillage
    int nLabels = connectedComponents(seuillage, labelImage, 8);


    // Fixage de la couleur pour chaque CC
    std::vector<Vec3b> colors(nLabels);
    colors[0] = Vec3b(0, 0, 0);//background
    for (int label = 1; label < nLabels; ++label) {
        colors[label] = Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
    }
    Mat dst(img.size(), CV_8UC3);
    for (int r = 0; r < dst.rows; ++r) {
        for (int c = 0; c < dst.cols; ++c) {

            int label = labelImage.at<int>(r, c);
            Vec3b& pixel = dst.at<Vec3b>(r, c);
            pixel = colors[label];
        }
    }


    //mettre les valeurs du label en 0
    int nb[nLabels];
    for (int i = 0; i < nLabels; i++) {
        nb[i] = 0;
    }

    // voir si l'image de ref et l'image bw ont le même composant connexe 
    for (int r = 0; r < dst.rows; ++r) {
        for (int c = 0; c < dst.cols; ++c) {

            if (seuillage.at<uchar>(r, c) == 255 && image.at<uchar>(r, c) == 255) {
                int label = labelImage.at<int>(r, c);
                nb[label] = 1;
            }


        }
    }


    // enlever les composantes connexes qui ne sont ni dans ref et ni dans bw
   // Mat dst(img.size(), CV_8UC3);
    for (int r = 0; r < dst.rows; ++r) {
        for (int c = 0; c < dst.cols; ++c) {

            int label = labelImage.at<int>(r, c);
            if (nb[label] == 0) {
                Vec3b& pixel = dst.at<Vec3b>(r, c);
                pixel = colors[0];
            }

        }
    }


    //ajoute la couleur blanc de ref dans dst
    for (int r = 0; r < dst.rows; ++r) {
        for (int c = 0; c < dst.cols; ++c) {

            if ((int)dst.at<Vec3b>(r, c)[0] == 0) {
                dst.at<Vec3b>(r, c)[0] = image.at<uchar>(r, c);
                dst.at<Vec3b>(r, c)[1] = image.at<uchar>(r, c);
                dst.at<Vec3b>(r, c)[2] = image.at<uchar>(r, c);
            }

        }
    }



    return dst;


}

//Méthode d'otsu qui prend une image en paramètre et retourne son seuil
int Otsu(Mat img) {
    int height = img.rows; 
    int width = img.cols;
    float histogram[256] = { 0 };
    for (int i = 0; i < height; i++)
    {
        unsigned char* p = (unsigned char*)img.data + img.step * i;
        for (int j = 0; j < width; j++)
        {
            histogram[*p++]++;
        }
    }
    int size = height * width;
    for (int i = 0; i < 256; i++)
    {
        histogram[i] = histogram[i] / size;
    }
    float avgValue = 0;
    for (int i = 0; i < 256; i++)
    {
        avgValue += i * histogram[i];
    }
    int threshold;
    float maxVariance = 0;
    float w = 0, u = 0;
    for (int i = 0; i < 256; i++)
    {
        w += histogram[i];
        u += i * histogram[i];
        float t = avgValue * w - u;
        float variance = t * t / (w * (1 - w));
        if (variance > maxVariance)
        {
            maxVariance = variance;
            threshold = i;
        }
    }
    return threshold;
}

//recherche le maximum dans une liste
//choix : stoppe la boucle while
//divise : la taille de la liste 
//liste : un tableau de valeur entières 
// maxpic : avoir le max de l'indice indiceMax
//indiceMax :  avoir l'indice où se situe le max
// i : aide pour la boucle 
void rechercheMax(bool choix, int divise, int liste[], int& maxpic1, int& indiceMax, int& i) {
    while (choix && i < divise) {
        if (maxpic1 < liste[i]) {
            
            maxpic1 = liste[i];
            if (maxpic1 > liste[i + 1]) {
                indiceMax = i;
                choix = false;
            }
            
        }

        i++;
    }


}

//recherche le minimum dans une liste
//choix : stoper la boucle while
//divise : la taille de la liste 
//liste : un tableau de valeur entier 
// maxpic : avoir le min de l'indice indiceMax
//indiceMax :  avoir l'indice où se situe le min
// i  : aide pour la boucle 
void rechercheMin(bool choix, int divise, int liste[], int& minpic1, int& indiceMin, int& i) {
    while (choix && i < divise) {
        if (minpic1 > liste[i]) {
            minpic1 = liste[i];
            if (minpic1 < liste[i + 1]) {
                indiceMin = i;
                choix = false;
            }
        }
        i++;

    }


}

//avoir les courbes des CC new et des Cc évoluantes 
//graphe : mettre les valeurs 
//divise :  la taille de la liste 
//max : le format de l'image 
//pas : combien de pas du début à la fin
//indice1  : aide à avoir l'indice dans une liste de taille divise
//listeaffCCagrandit :  liste des CC qui grandissent 
//listeafficheCC:  liste des nouvelles CC 
// seuil : le seuil (otsu ou non) 
//indiceMi: deuxieme indice minimum dans la liste des CC qui ont grandit 
//indiceMa: deuxieme indice maximum  dans la liste des CC qui ont grandit 
//indiceMin: premier indice minimume dans la liste des CC qui ont grandit  
//indiceMin1: premier indice minimume dans la liste des CC qui ont grandit 
void faireGraphe(Mat graphe, int divise, int max, int pas, int indice1, int listeaffCCagrandit[], int listeafficheCC[], int seuil, int indiceMi, int indiceMa, int indiceMin, int indiceMin1) {

    for (int i = 0; i < 500; i = i + ((pas * 500) / 256)) {

        //affiche la courbe de nouvelles CC
        if (indice1 + 1 < divise) {

            //graphe.at<uchar>(500-((listeafficheCC[indice1]*500)/max),i+1)=255;
            if (indice1 < indiceMin) {
                graphe.at<Vec3b>(500 - ((listeafficheCC[indice1] * 500) / max), i + 1)[0] = 200;
                graphe.at<Vec3b>(500 - ((listeafficheCC[indice1] * 500) / max), i + 1)[1] = 100;
                graphe.at<Vec3b>(500 - ((listeafficheCC[indice1] * 500) / max), i + 1)[2] = 100;

            }
            else {
                graphe.at<Vec3b>(500 - ((listeafficheCC[indice1] * 500) / max), i + 1)[0] = 255;
                graphe.at<Vec3b>(500 - ((listeafficheCC[indice1] * 500) / max), i + 1)[1] = 255;
                graphe.at<Vec3b>(500 - ((listeafficheCC[indice1] * 500) / max), i + 1)[2] = 0;

            }


            //affiche la courbe de CC qui ont grandi 
            if (indice1 >= indiceMin1 && indice1 > indiceMi) {
                graphe.at<Vec3b>(500 - ((listeaffCCagrandit[indice1] * 500) / max), i + 1)[0] = 255;
                graphe.at<Vec3b>(500 - ((listeaffCCagrandit[indice1] * 500) / max), i + 1)[1] = 255;
                graphe.at<Vec3b>(500 - ((listeaffCCagrandit[indice1] * 500) / max), i + 1)[2] = 255;

            }
            else {
                graphe.at<Vec3b>(500 - ((listeaffCCagrandit[indice1] * 500) / max), i + 1)[0] = 255;
                graphe.at<Vec3b>(500 - ((listeaffCCagrandit[indice1] * 500) / max), i + 1)[1] = 0;
                graphe.at<Vec3b>(500 - ((listeaffCCagrandit[indice1] * 500) / max), i + 1)[2] = 255;

            }


            //On relie les points pour créer une courbe//

            int aide = i + 1 + ((pas * 500) / 256);
            //New CC 
            if (indice1 < indiceMin) {
                line(graphe, Point(i + 1, 500 - ((listeafficheCC[indice1] * 500) / max)), Point(aide, 500 - ((listeafficheCC[indice1 + 1] * 500) / max)), Scalar(200, 100, 100), 1, LINE_4);
            }
            else {
                line(graphe, Point(i + 1, 500 - ((listeafficheCC[indice1] * 500) / max)), Point(aide, 500 - ((listeafficheCC[indice1 + 1] * 500) / max)), Scalar(255, 255, 0), 1, LINE_4);
            }
            //CC s'étant agrandies  
            if (indice1 >= indiceMin1 && indice1 < indiceMi) {
                line(graphe, Point(i + 1, 500 - ((listeaffCCagrandit[indice1] * 500) / max)), Point(aide, 500 - ((listeaffCCagrandit[indice1 + 1] * 500) / max)), Scalar(255, 255, 255), 1, LINE_4);

            }
            else {
                line(graphe, Point(i + 1, 500 - ((listeaffCCagrandit[indice1] * 500) / max)), Point(aide, 500 - ((listeaffCCagrandit[indice1 + 1] * 500) / max)), Scalar(255, 0, 255), 1, LINE_4);

            }

            indice1++;

        }

    }
    cout << "Fin d'édition du graphique." << endl;
    // affiche la droite du seuil d'Otsu
    line(graphe, Point((seuil * 500) / 256, 0), Point((seuil * 500) / 256, 500), Scalar(0, 255, 0), 1, LINE_4);

}


int main(int argc, const char** argv) {

    /***********Mettre tous les tableaux à 0************/
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            nbConEachS[i][j] = 0;
        }

    }
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            nbNewCC[i][j] = 0;
        }

    }

    /*******************Avoir l'image **********************/

    img = imread("/Users/adamdiakite/Desktop/typed-handwritten-aurelie/TER/test.jpg",IMREAD_GRAYSCALE);
    waitKey(0);

    if (img.empty())
    {
        cout << "Could not read input image file: " << img << endl;
        return EXIT_FAILURE;
    }

    /*****************faire le seuillage *********************/
    threshold(img, otsu, 0, 255, THRESH_BINARY | THRESH_OTSU);
    otsu = ~otsu;
    Mat binary;
    int seuil = 0;
    seuil = Otsu(img);
    Mat imprime;

    cout << endl;

    /***************************Obtention des CC***********************************************/

    //avoir les new CC et les CC qui grandissent 
    int pas = 10;
    int divise = 256 / pas + 1;
    cout << "\nDivise : " << divise << endl;
    cout << "pas : " << pas << endl;
    avoirNCC(256, 0, pas);

    cout << " Les CC qui appartiennent eu blanc" << endl;

    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            if (nbConEachS[i][j] != 0 && nbConEachS[i][j] != 1) {
                cout << "tab [" << i << "," << j << "]" << " : " << nbConEachS[i][j] << " | ";
            }

        }
    }
    cout << endl;
    cout << "Obtention des nouvelles CC uniquement" << endl;
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            if (nbNewCC[i][j] > 1) {
                cout << "tab [" << i << "," << j << "]" << " : " << nbNewCC[i][j] << " | ";
            }

        }
    }

    //avoir le nombre de nouvelles CC et CC aggrandies
    int listeafficheCC[divise];
    int listeaffCCagrandit[divise];

    for (int i = 0; i < divise; i++) {
        listeafficheCC[i] = 0;
        listeaffCCagrandit[i] = 0;
    }

    int indice = 0;
    for (int i = 0; i < 256; i = i + pas) {
        if (i + pas < 256) {
            listeafficheCC[indice] = nbNewCC[i][i + pas];
            listeaffCCagrandit[indice] = nbConEachS[i][i + pas];
            indice++;
        }

    }
    cout << "\nAffichage de la liste de CC" << endl;
    for (int i = 0; i < divise; i++) {
        cout << "tab [" << i << "]" << " : " << listeafficheCC[i] << " | ";

    }
    cout << "\nAffichage de la listes de nouvelles CC" << endl;
    for (int i = 0; i < divise; i++) {
        cout << "tab [" << i << "]" << " : " << listeaffCCagrandit[i] << " | ";

    }

    /**************Recherche du premier pic d'apparition de pixels = Imprimé ************************/

    //avoir le max de la liste de nouvelles cc 
    int maxCCA = maximum(listeaffCCagrandit, divise);
    //Avoir le max de la liste de cc grandissantes 
    int maxNCC = maximum(listeafficheCC, divise);

    int maxpic2 = listeaffCCagrandit[0];
    int maxpic1 = listeafficheCC[0];
    int indiceMax = 0;
    bool choix = true;
    int i = 0;
    int j = 0;
    //cout<<"\ndebut"<<endl;
    //trouver le premier pic max dans le tab CC nouvelles 

    rechercheMax(choix, divise, listeafficheCC, maxpic1, indiceMax, i);

    int indiceMax2 = 0;
    bool choix1 = true;
    //avoir le premier pic max dans CC agrandies 

    rechercheMax(choix1, divise, listeaffCCagrandit, maxpic2, indiceMax2, j);

    cout << "\nmax pic1 : " << maxpic1 << endl;
    cout << "\nmax pic2 : " << maxpic2 << endl;
    int minpic1 = maxpic1;
    int minpic2 = maxpic2;
    int indiceMin = 0;
    int indiceMin1 = 0;
    choix = true;
    choix1 = true;

    //avoir le premier minimum dans CC nouvelles
    rechercheMin(choix, divise, listeafficheCC, minpic1, indiceMin, i);

    //avoir le premier minimum dans le CC grandissantes

    rechercheMin(choix1, divise, listeaffCCagrandit, minpic2, indiceMin1, j);

    cout << "\nmin pic1 : " << minpic1 << endl;
    cout << "\nmin pic2 : " << minpic2 << endl;



    int maxpic = minpic2;
    int l = j;
    bool c = true;
    int indiceMi = 0;
    int indiceMa = 0;

    //avoir le second pic max dans le CC agrandit 
    rechercheMax(c, divise, listeaffCCagrandit, maxpic, indiceMa, l);
    c = true;
    int minpic = maxpic;
    //avoir le deuxieme pic min dans le CCagrandit 
    rechercheMin(c, divise, listeaffCCagrandit, minpic, indiceMi, l);

    cout << indiceMax << " " << indiceMin << endl;
    cout << indiceMax2 << " " << indiceMin1 << endl;

    cout << "\n min pic " << minpic << " max pic " << maxpic << endl;
    cout << indiceMa << " " << indiceMi << endl;

    if (indiceMi == 0) {
        int aide = seuil / pas;
        indiceMi = aide + 1;
        cout << "changement du 0 de indiceMi" << indiceMi << endl;
    }

    /*********************Faire le Graphe **************************************/


    cout << " maxNCC : " << maxNCC << endl;
    cout << " maxCCA : " << maxCCA << endl;
    //verifier la taille pour la fenetre du graphe 
    int max = 0;
    if (maxNCC <= maxCCA) {
        max = maxCCA;
    }
    else {
        max = maxNCC;
    }
    cout << "max :" << max << endl;
    Mat graphe = Mat::zeros(500, 500, CV_8UC3);
    //graphe=~graphe;
    int indice1 = 0;

    faireGraphe(graphe, divise, max, pas, indice1, listeaffCCagrandit, listeafficheCC, seuil, indiceMi, indiceMa, indiceMin, indiceMin1);
    cout << "En vert, le seuil d'Otsu : " << seuil << ",En Cyan les nouvelles CC et en violet les CC grandissantes " << endl;
    imshow("Graphe d'apparition des composantes connexes", graphe);
    waitKey(0);
    cout << endl;


    /**************Affichage de l'imprimer et du manuscrit************************/

    int sDebutCC = 0, sFinCC = indiceMin;
    int sDebutCCAgrandit = indiceMin1, sFinCCagrandit = indiceMa;

    Mat imprimer = Mat::zeros(img.rows, img.cols, CV_8UC1);
    Mat manuscrit = Mat::zeros(img.rows, img.cols, CV_8UC1);

    //obtention des CC imprimées
    for (int i = sDebutCC; i < sFinCC; i++) {


        Mat bw = i * pas + pas < 256 ? (img < (i* pas + pas)) : (img > (i * pas + pas));
        Mat ref = i * pas < 256 ? (img < i* pas) : (img > i * pas);


        Mat diff = avoirDiff(bw, ref);

        for (int i = 0; i < diff.rows; i++) {
            for (int j = 0; j < diff.cols; j++) {
                if (imprimer.at<uchar>(i, j) == 0 && diff.at<uchar>(i, j) == 255) {
                    imprimer.at<uchar>(i, j) = 255;
                }
            }
        }

    }

    //obtention des cc manuscrites
    for (int i = sDebutCCAgrandit; i < sFinCCagrandit; i++) {


        Mat bw = i * pas + pas < 256 ? (img < (i* pas + pas)) : (img > (i * pas + pas));
        Mat ref = i * pas < 256 ? (img < i* pas) : (img > i * pas);

        Mat diff = avoirDiff(bw, ref);

        for (int i = 0; i < diff.rows; i++) {
            for (int j = 0; j < diff.cols; j++) {
             
                if (manuscrit.at<uchar>(i, j) == 0 && diff.at<uchar>(i, j) == 255) {
                    manuscrit.at<uchar>(i, j) = 255;
                }
            }
        }

    }

    imshow("Manuscrit", manuscrit);
    imshow("Imprimé", imprimer);
    imshow("Otsu", otsu);

    /*******************fusion du manuscrit et de l'imprimé*******************************/
    cvtColor(manuscrit, manuscrit, 1);
    cvtColor(imprimer, imprimer, 1);

    Mat resultat = Mat::zeros(img.rows, img.cols, CV_8UC3);
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            if (manuscrit.at<Vec3b>(i, j)[0] == 255 && imprimer.at<Vec3b>(i, j)[0] == 255) {
                resultat.at<Vec3b>(i, j)[0] = 255;
                resultat.at<Vec3b>(i, j)[1] = 255;
                resultat.at<Vec3b>(i, j)[2] = 255;
            }
            else {
                if (manuscrit.at<Vec3b>(i, j)[0] == 255 && imprimer.at<Vec3b>(i, j)[0] == 0) {
                    //violet
                    resultat.at<Vec3b>(i, j)[0] = 255;
                    resultat.at<Vec3b>(i, j)[1] = 0;
                    resultat.at<Vec3b>(i, j)[2] = 155;
                }
                else {
                    if (imprimer.at<Vec3b>(i, j)[0] == 255 && manuscrit.at<Vec3b>(i, j)[0] == 0) {
                        //jaune
                        resultat.at<Vec3b>(i, j)[0] = 0;
                        resultat.at<Vec3b>(i, j)[1] = 255;
                        resultat.at<Vec3b>(i, j)[2] = 255;
                    }
                }
            }

        }
    }
    // en jaune : imprimer et en violet : manuscrit 
    imshow("resultat", resultat);
    waitKey(0);

    /********************CC dans l'image******************************/

    Mat labelImage(img.size(), CV_32S);
    int nLabels = connectedComponents(otsu, labelImage, 8);
    int nbCCPixel[nLabels];
    int nbJaune[nLabels];
    int nbViolet[nLabels];
    int verif[nLabels];
    int verif2[nLabels];

    std::vector<Vec3b> colors(nLabels);
    colors[0] = Vec3b(0, 0, 0);//background
    nbCCPixel[0] = 0;
    nbJaune[0] = 0;
    nbViolet[0] = 0;
    verif[0] = 2;
    verif2[0] = 2;
    for (int label = 1; label < nLabels; ++label) {
        colors[label] = Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
        nbCCPixel[label] = 0;
        nbJaune[label] = 0;
        nbViolet[label] = 0;
        verif[label] = 0;
        verif2[label] = 2;
    }
    Mat dst(img.size(), CV_8UC3);
    for (int r = 0; r < dst.rows; ++r) {
        for (int c = 0; c < dst.cols; ++c) {
            int label = labelImage.at<int>(r, c);
            if (resultat.at<Vec3b>(r, c)[0] == 255 && resultat.at<Vec3b>(r, c)[1] == 0 && resultat.at<Vec3b>(r, c)[2] == 155) {
                nbViolet[label]++;
            }
            else {
                if (resultat.at<Vec3b>(r, c)[0] == 0 && resultat.at<Vec3b>(r, c)[1] == 255 && resultat.at<Vec3b>(r, c)[2] == 255) {
                    nbJaune[label]++;
                }
            }

            if (manuscrit.at<Vec3b>(r, c)[0] == 255 && imprimer.at<Vec3b>(r, c)[0] == 0) {
                //violet
                if (verif2[label] == 2) {
                    verif2[label] = 1;
                }

            }
            else {
                if (imprimer.at<Vec3b>(r, c)[0] == 255 && manuscrit.at<Vec3b>(r, c)[0] == 0) {
                    //jaune
                    if (verif2[label] == 2) {
                        verif2[label] = 0;
                    }

                }
            }
            nbCCPixel[label]++;
            Vec3b& pixel = dst.at<Vec3b>(r, c);
            pixel = colors[label];
        }
    }
    imshow("Composantes Otsu", dst);
    waitKey(0);

    /************************le résultat final ***********************************/
    //si 0 pour jaune et 1 pour violet, 2 pour le background
    for (int i = 1; i < nLabels; i++) {
        if (nbViolet[i] > nbJaune[i]) {
            verif[i] = 1;
        }
    }

    Mat jaune = Mat::zeros(img.rows, img.cols, CV_8UC3);
    Mat violet = Mat::zeros(img.rows, img.cols, CV_8UC3);
    Mat Finalresultat = Mat::zeros(img.rows, img.cols, CV_8UC3);
    for (int r = 0; r < Finalresultat.rows; ++r) {
        for (int c = 0; c < Finalresultat.cols; ++c) {
            int label = labelImage.at<int>(r, c);

            if (verif[label] == 0) {
                //jaune
                Finalresultat.at<Vec3b>(r, c)[0] = 0;
                Finalresultat.at<Vec3b>(r, c)[1] = 255;
                Finalresultat.at<Vec3b>(r, c)[2] = 255;
                jaune.at<Vec3b>(r, c)[0] = 0;
                jaune.at<Vec3b>(r, c)[1] = 255;
                jaune.at<Vec3b>(r, c)[2] = 255;


            }
            else {
                //violet
                if (verif[label] == 1) {
                    Finalresultat.at<Vec3b>(r, c)[0] = 255;
                    Finalresultat.at<Vec3b>(r, c)[1] = 0;
                    Finalresultat.at<Vec3b>(r, c)[2] = 155;
                    violet.at<Vec3b>(r, c)[0] = 255;
                    violet.at<Vec3b>(r, c)[1] = 0;
                    violet.at<Vec3b>(r, c)[2] = 155;
                }
            }
        }
    }

    imshow("Résultat Final fusionné", Finalresultat);
    imshow("Violet",violet);
    imshow("Jaune",jaune);
    waitKey(0);



    /*********************Affichage uniquement des nouvelles CC*****************/
    namedWindow("Composantes connexes", WINDOW_AUTOSIZE);
    createTrackbar("Seuillage", "Composantes connexes", &threshval, 255, on_trackbar);
    createTrackbar("R�f�rence", "Composantes connexes", &t, 255, myCallbackForT);
    on_trackbar(threshval, 0);
    waitKey(0);



    return 0;
}