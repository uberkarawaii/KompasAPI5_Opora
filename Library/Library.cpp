////////////////////////////////////////////////////////////////////////////////
//
// Library.cpp - Библиотека на Visual C++
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <afxdllx.h>
#include <iostream>
#include "Library.h"
#include "KompasForm.h"
#include <comutil.h>
#include <string>

 

#ifdef _DEBUG
#undef THIS_FILE

/*далее сгенерировано было просто в одну строку: static char THIS_FILE[] = __FILE__;
но это вызывало ошибку: недопустимый пустой инициализатор для массива с незаданной привязкой
поэтому переделываю на этот вариант*/

#include <cstring>

static char THIS_FILE[__FILE__[0] ? sizeof(__FILE__) : 1]; // выделяем массив по длине строки
struct ThisFileInitializer {
    ThisFileInitializer() {
        std::memcpy(THIS_FILE, __FILE__, sizeof(__FILE__)); // включая '\0'
    }
} thisFileInitializer;

#endif

const double M_PI = 3.141592653589793;
 

//-------------------------------------------------------------------------------
// Специальная структура, используемая в течении инициализации DLL
// ---
static AFX_EXTENSION_MODULE LibraryDLL = { NULL, NULL };
HINSTANCE g_hInstance = NULL;


KompasObjectPtr kompas(NULL);
  
void OnProcessDetach();


//-------------------------------------------------------------------------------
// Стандартная точка входа
// Инициализация и завершение DLL
// ---
extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    g_hInstance = hInstance;

    if (dwReason == DLL_PROCESS_ATTACH)
    {
        TRACE0("Library.AWX Initializing!\n");

        AfxInitExtensionModule(LibraryDLL, hInstance);

        new CDynLinkLibrary(LibraryDLL);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        TRACE0("Library.AWX Terminating!\n");
        
            OnProcessDetach();
        
            AfxTermExtensionModule(LibraryDLL);
    }
    return 1;   // ok
}

//-------------------------------------------------------------------------------
//
// ---
void GetKompas()
{
    if (!(bool)kompas)
    {
        CString filename;

        if (::GetModuleFileName(NULL, filename.GetBuffer(255), 255))
        {
            filename.ReleaseBuffer(255);
            CString libname;

            libname = LoadStr(IDR_AUTO_LIB);  //kAPI5.dll

            filename.Replace(filename.Right(filename.GetLength() - (filename.ReverseFind('\\') + 1)),
                libname);

            // идентификатор kAPI5.dll
            // HINSTANCE hAppAuto = LoadLibrary(_T(filename)); - или около того генерируется LibraryWizard2022
            // при данном варианте была ошибка при открытии компаса. когда путь абсолютный, ошибки нет
            HINSTANCE hAppAuto = LoadLibrary(_T("C:\\Program Files\\ASCON\\KOMPAS-3D v23 Study\\Bin\\kAPI5.dll")); 

            if (hAppAuto) {
                typedef LPDISPATCH(WINAPI* FCreateKompasObject)();
                FCreateKompasObject pCreateKompasObject =
                    (FCreateKompasObject)GetProcAddress(hAppAuto, "CreateKompasObject");
                if (pCreateKompasObject)
                    kompas = IDispatchPtr(pCreateKompasObject(), false/*AddRef*/);

                FreeLibrary(hAppAuto);
            }
        }
    }
}
 

//-------------------------------------------------------------------------------
// Задать идентификатор ресурсов
// ---
unsigned int WINAPI LIBRARYID()
{
    return IDR_LIBID;
}

// ф-ция для изменения цвета. не работает, возможно из-за монохромного режима в компасе
void SetEntityColor(ksPartPtr entity, long color)
{
    if (!entity) return;

    double ambient = 0.5;
    double diffuse = 0.6;
    double specularity = 0.8;
    double shininess = 0.8;
    double transparency = 0.0;
    double emission = 0.0;

    std::cout << entity->SetAdvancedColor(color, ambient, diffuse, specularity, shininess, transparency, emission) << std::endl;
}

// крепёжные элементы
void Additions(ksPartPtr Part1) {

    // == БОЛТ ==
    ksEntityPtr basePlaneYOZ_Torus = Part1->GetDefaultEntity(o3d_planeYOZ);

    // создаём смещённую плоскость на по X
    ksEntityPtr offsetPlane_Torus = Part1->NewEntity(o3d_planeOffset);
    ksPlaneOffsetDefinitionPtr offsetDef_Torus = offsetPlane_Torus->GetDefinition();
    offsetDef_Torus->direction = FALSE; // Отрицательное направление X
    offsetDef_Torus->offset = 17.5;
    offsetDef_Torus->SetPlane(basePlaneYOZ_Torus);
    offsetPlane_Torus->Create();

    // Создаём эскиз на новой плоскости
    ksEntityPtr circleSketch_Torus = Part1->NewEntity(o3d_sketch);
    ksSketchDefinitionPtr circleDef_Torus = circleSketch_Torus->GetDefinition();
    circleDef_Torus->SetPlane(offsetPlane_Torus);
    circleSketch_Torus->Create();

    ksDocument2DPtr circleDoc_Torus = circleDef_Torus->BeginEdit();

    // задаём параметры окружности
    double circleCenterY_Torus = -24.4;      // высота (ось Y)
    double circleCenterZ_Torus = -25.0;      // по центру ширины (ось Z)
    double circleRadius_Torus = 5.0;         // радиус 5 мм (диаметр 10 мм)

    circleDoc_Torus->ksCircle(circleCenterY_Torus, circleCenterZ_Torus, circleRadius_Torus, 1);
    circleDef_Torus->EndEdit();

    // вытягиваем окружность 
    ksEntityPtr circleExtrude_Torus = Part1->NewEntity(o3d_bossExtrusion);
    ksBossExtrusionDefinitionPtr circleExtrudeDef_Torus = circleExtrude_Torus->GetDefinition();
    circleExtrudeDef_Torus->SetSketch(circleSketch_Torus);
    circleExtrudeDef_Torus->directionType = dtNormal;
    circleExtrudeDef_Torus->SetSideParam(TRUE, etBlind, 45.0, 0.0, FALSE);
    circleExtrude_Torus->Create();


    // == ГАЙКА ==
    ksEntityPtr basePlaneYOZ_Hex = Part1->GetDefaultEntity(o3d_planeYOZ);

    // создаём смещённую плоскость на -15 мм по X
    ksEntityPtr offsetPlane_Hex = Part1->NewEntity(o3d_planeOffset);
    ksPlaneOffsetDefinitionPtr offsetDef_Hex = offsetPlane_Hex->GetDefinition();
    offsetDef_Hex->direction = FALSE; // отрицательное направление X
    offsetDef_Hex->offset = 15.0;    // смещение на 15 мм
    offsetDef_Hex->SetPlane(basePlaneYOZ_Hex);
    offsetPlane_Hex->Create();

    // создаём эскиз шестиугольника
    ksEntityPtr hexSketch = Part1->NewEntity(o3d_sketch);
    ksSketchDefinitionPtr hexDef = hexSketch->GetDefinition();
    hexDef->SetPlane(offsetPlane_Hex);
    hexSketch->Create();

    ksDocument2DPtr hexDoc = hexDef->BeginEdit();

    // параметры шестиугольника
    double hexCenterY = -24.4;
    double hexCenterZ = -25.0;
    double hexRadius = 10.0; // радиус описанной окружности
    double prevX = hexCenterY + hexRadius; // начинаем с 0 градусов
    double prevY = hexCenterZ;

    for (int i = 1; i <= 6; i++) {
        double angle = i * M_PI / 3.0; // 60 градусов в радианах
        double currX = hexCenterY + hexRadius * cos(angle);
        double currY = hexCenterZ + hexRadius * sin(angle);

        hexDoc->ksLineSeg(prevX, prevY, currX, currY, 1); // стиль 1 - обычная линия

        prevX = currX;
        prevY = currY;
    }

    // Добавляем центральное отверстие диаметром 10.2 мм
    hexDoc->ksCircle(hexCenterY, hexCenterZ, 5.1, 1); // Радиус 5.1 мм

    hexDef->EndEdit();

    // Вытягиваем шестиугольник на 5 мм
    ksEntityPtr hexExtrude = Part1->NewEntity(o3d_bossExtrusion);
    ksBossExtrusionDefinitionPtr hexExtrudeDef = hexExtrude->GetDefinition();
    hexExtrudeDef->SetSketch(hexSketch);
    hexExtrudeDef->directionType = dtNormal;
    hexExtrudeDef->SetSideParam(TRUE, etBlind, 5.0, 0.0, FALSE);
    hexExtrude->Create();


    //== ШАЙБА ==
    ksEntityPtr basePlaneYOZ_Rings = Part1->GetDefaultEntity(o3d_planeYOZ);
    // Создаём смещённую плоскость на 25 мм по X
    ksEntityPtr offsetPlane_Rings = Part1->NewEntity(o3d_planeOffset);
    ksPlaneOffsetDefinitionPtr offsetDef_Rings = offsetPlane_Rings->GetDefinition();
    offsetDef_Rings->direction = TRUE; // Положительное направление X
    offsetDef_Rings->offset = 25.0;    // Смещение на 25 мм
    offsetDef_Rings->SetPlane(basePlaneYOZ_Rings);
    offsetPlane_Rings->Create();

    // Создаём эскиз колец
    ksEntityPtr ringsSketch = Part1->NewEntity(o3d_sketch);
    ksSketchDefinitionPtr ringsDef = ringsSketch->GetDefinition();
    ringsDef->SetPlane(offsetPlane_Rings);
    ringsSketch->Create();

    ksDocument2DPtr ringsDoc = ringsDef->BeginEdit();

    // Центр кругов
    double ringsCenterY = -24.4;
    double ringsCenterZ = -25.0;

    // внешний круг
    ringsDoc->ksCircle(ringsCenterY, ringsCenterZ, 8.0, 1); // радиус 8 мм

    // внутренний круг
    ringsDoc->ksCircle(ringsCenterY, ringsCenterZ, 5.0, 1);

    ringsDef->EndEdit();

    // вытягивание на 1.6 мм
    ksEntityPtr ringsExtrude = Part1->NewEntity(o3d_bossExtrusion);
    ksBossExtrusionDefinitionPtr ringsExtrudeDef = ringsExtrude->GetDefinition();
    ringsExtrudeDef->SetSketch(ringsSketch);
    ringsExtrudeDef->directionType = dtNormal;
    ringsExtrudeDef->SetSideParam(TRUE, etBlind, 1.6, 0.0, FALSE);
    ringsExtrude->Create();

}

//--------------------------------------------------------------------------------
// создание опоры, осн функция
// ---
void CreateCylinder(int flag)
{
    // попытка подключения к компасу, с перехватом возможных ошибок
    try {
        GetKompas();
        if (!kompas) {
            AfxMessageBox(_T("Не удалось подключиться к Компас-3D"));
            return;
        }

        ksDocument3DPtr iDoc3D = kompas->Document3D();
        if (!iDoc3D) {
            AfxMessageBox(_T("Не удалось создать 3D-документ"));
            return;
        }

        iDoc3D->Create(false, true);
        iDoc3D = kompas->ActiveDocument3D();

        ksPartPtr Part1 = iDoc3D->GetPart(pTop_Part);
        if (!Part1) {
            AfxMessageBox(_T("Не удалось получить часть документа"));
            return;
        }

        //== далее идёт построение опоры ==

        // === горизональный элемент с двумя прорезями ===
        ksEntityPtr sketchEntity = Part1->NewEntity(o3d_sketch);
        ksSketchDefinitionPtr sketchDef = sketchEntity->GetDefinition();
        sketchDef->SetPlane(Part1->GetDefaultEntity(o3d_planeXOY));
        sketchEntity->Create();

        ksDocument2DPtr doc2D = sketchDef->BeginEdit();

        // прямоугольник
        double width = 60.0;
        double height = 50.0;
        doc2D->ksLineSeg(0, 0, width, 0, 1);         // нижняя
        doc2D->ksLineSeg(width, 0, width, height, 1); // правая
        doc2D->ksLineSeg(width, height, 0, height, 1); // верхняя
        doc2D->ksLineSeg(0, height, 0, 0, 1);         // левая

        sketchDef->EndEdit();

        // вытягивание на 10 мм вверх
        ksEntityPtr extrude = Part1->NewEntity(o3d_bossExtrusion);
        ksBossExtrusionDefinitionPtr extrudeDef = extrude->GetDefinition();
        extrudeDef->SetSketch(sketchEntity);
        extrudeDef->directionType = dtNormal;
        extrudeDef->SetSideParam(TRUE, etBlind, 10.0, 0, FALSE);
        extrude->Create();

        SetEntityColor(Part1, 0x0000FF00);

        // === отверстия Ø11 ===
        double holeRadius = 5.5;
        double holeSpacing = 30.0;
        double x = 49.0, y1 = 10.0, y2 = 40.0;

        auto createHole = [&](double x, double y) {
            ksEntityPtr holeSketch = Part1->NewEntity(o3d_sketch);
            ksSketchDefinitionPtr holeDef = holeSketch->GetDefinition();
            holeDef->SetPlane(Part1->GetDefaultEntity(o3d_planeXOY));  // XOY
            holeSketch->Create();

            ksDocument2DPtr holeDoc = holeDef->BeginEdit();
            holeDoc->ksCircle(x, y, holeRadius, 1);
            holeDef->EndEdit();

            ksEntityPtr cut = Part1->NewEntity(o3d_cutExtrusion);
            ksCutExtrusionDefinitionPtr cutDef = cut->GetDefinition();
            cutDef->SetSketch(holeSketch);
            cutDef->directionType = dtReverse;
            cutDef->SetSideParam(TRUE, etThroughAll, 0, 0, FALSE);  // Сквозное
            cut->Create();
            };

        // два отверстия с отступом 11 и расстоянием 30 между ними
        createHole(x, y1);
        createHole(x, y2);

        // === ТРАПЕЦИЯ НА БОКОВОЙ ГРАНИ (YOZ, X=0) ===
        // базовая плоскость YOZ
        ksEntityPtr basePlaneYOZ = Part1->GetDefaultEntity(o3d_planeYOZ);

        // создаём смещённую плоскость на 10 мм по X
        ksEntityPtr offsetPlane = Part1->NewEntity(o3d_planeOffset);
        ksPlaneOffsetDefinitionPtr offsetDef = offsetPlane->GetDefinition();
        offsetDef->direction = TRUE; // Направление смещения: вдоль X
        offsetDef->offset = -10.0;    // Смещение на 10 мм
        offsetDef->SetPlane(basePlaneYOZ);
        offsetPlane->Create();

        // эскиз на смещённой плоскости
        ksEntityPtr trapezoidSketch = Part1->NewEntity(o3d_sketch);
        ksSketchDefinitionPtr trapezoidDef = trapezoidSketch->GetDefinition();
        trapezoidDef->SetPlane(offsetPlane);
        trapezoidSketch->Create();

        ksDocument2DPtr trapezoidDoc = trapezoidDef->BeginEdit();

        // Параметры трапеции
        double yBottom = -10.0;     
        double yTop = -30.0;        
        double height1 = 20.0;
        // Ширины 
        double zLeft = 0.0;
        double zRight = -50.0;
        double zInnerLeft = -10.0;
        double zInnerRight = -40.0;

        // трапеция осн. линии
        trapezoidDoc->ksLineSeg(yBottom, zLeft, yTop, zInnerLeft, 1);    
        trapezoidDoc->ksLineSeg(yTop, zInnerLeft, yTop, zInnerRight, 1); 
        trapezoidDoc->ksLineSeg(yTop, zInnerRight, yBottom, zRight, 1);  
        trapezoidDoc->ksLineSeg(yBottom, zRight, yBottom, zLeft, 1);     

        // Круг радиусом 10 на высоте 10 от основания
        double circleRadius = 15.8;
        double yMiddle = -25; 
        double zCenter = -25.0;

        trapezoidDoc->ksCircle(yMiddle, zCenter, circleRadius, 1);  // Круг в середине
        trapezoidDef->EndEdit();

        ksEntityPtr trapezoidExtrude = Part1->NewEntity(o3d_bossExtrusion);
        ksBossExtrusionDefinitionPtr trapezoidExtrudeDef = trapezoidExtrude->GetDefinition();
        trapezoidExtrudeDef->SetSketch(trapezoidSketch);
        trapezoidExtrudeDef->directionType = dtNormal;
        trapezoidExtrudeDef->SetSideParam(TRUE, etBlind, 10.0, 0.0, FALSE);
        trapezoidExtrude->Create();


        // === ОКРУЖНОСТЬ НА БОКОВОЙ ГРАНИ (YOZ, X=0) ===
        ksEntityPtr circleSketch = Part1->NewEntity(o3d_sketch);
        ksSketchDefinitionPtr circleDef = circleSketch->GetDefinition();
        circleDef->SetPlane(Part1->GetDefaultEntity(o3d_planeYOZ));  // Плоскость YOZ
        circleSketch->Create();

        ksDocument2DPtr circleDoc = circleDef->BeginEdit();

        // Центр окружности — по координатам Y и Z
        double centerY = -24.4;     // Высота
        double centerZ = -25.0;    // По центру ширины
        double radius = 29.5;

        circleDoc->ksCircle(centerY, centerZ, radius, 1);
        circleDef->EndEdit();

        //вытягивание окр
        ksEntityPtr circleExtrude = Part1->NewEntity(o3d_bossExtrusion);
        ksBossExtrusionDefinitionPtr circleExtrudeDef = circleExtrude->GetDefinition();
        circleExtrudeDef->SetSketch(circleSketch);
        circleExtrudeDef->directionType = dtNormal;
        circleExtrudeDef->SetSideParam(TRUE, etBlind, 20.0, 0.0, FALSE);
        circleExtrude->Create();

        // === ВТОРАЯ ОКРУЖНОСТЬ НА СМЕЩЁННОЙ ПЛОСКОСТИ (YOZ, X=20) ===
        ksEntityPtr basePlaneYOZ2 = Part1->GetDefaultEntity(o3d_planeYOZ);

        // Создаём смещённую плоскость на 20 мм по X
        ksEntityPtr offsetPlane2 = Part1->NewEntity(o3d_planeOffset);
        ksPlaneOffsetDefinitionPtr offsetDef2 = offsetPlane2->GetDefinition();
        offsetDef2->direction = TRUE; // Вдоль положительного направления X
        offsetDef2->offset = 20.0;    // Смещение на 20 мм
        offsetDef2->SetPlane(basePlaneYOZ2);
        offsetPlane2->Create();

        // эскиз на новой плоскости
        ksEntityPtr circleSketch2 = Part1->NewEntity(o3d_sketch);
        ksSketchDefinitionPtr circleDef2 = circleSketch2->GetDefinition();
        circleDef2->SetPlane(offsetPlane2);
        circleSketch2->Create();

        ksDocument2DPtr circleDoc2 = circleDef2->BeginEdit();

        // параметры окружности
        double circleCenterY2 = -24.4;      // Высота (ось Y)
        double circleCenterZ2 = -25.0;      // По центру ширины (ось Z)
        double circleRadius2 = 14.75;

        circleDoc2->ksCircle(circleCenterY2, circleCenterZ2, circleRadius2, 1);
        circleDef2->EndEdit();

        // Вытягиваем окружность на 20 мм
        ksEntityPtr circleExtrude2 = Part1->NewEntity(o3d_bossExtrusion);
        ksBossExtrusionDefinitionPtr circleExtrudeDef2 = circleExtrude2->GetDefinition();
        circleExtrudeDef2->SetSketch(circleSketch2);
        circleExtrudeDef2->directionType = dtNormal;
        circleExtrudeDef2->SetSideParam(TRUE, etBlind, 5.0, 0.0, FALSE);
        circleExtrude2->Create();

        // ТЕЛО КОТОРОЕ ДОЛЖНО БЫТЬ СО СКРУГЛЕНИЕМ, но скругления не вышло
        // скруглять можно только выбрав операцию "скругление" в дереве построения и нажав на нужные грани.
        // брать последние грани при вытягивании компас упорно не хочет
        // т.е. вывод сообщения "скруглено 6 рёбер" есть, а на самом деле операция не происходит
        
        // 1. Смещённая плоскость на 10 мм от YOZ
        ksEntityPtr baseYOZ4 = Part1->GetDefaultEntity(o3d_planeYOZ);
        ksEntityPtr offsetPlane4 = Part1->NewEntity(o3d_planeOffset);
        ksPlaneOffsetDefinitionPtr offsetDef4 = offsetPlane4->GetDefinition();
        offsetDef4->direction = TRUE;
        offsetDef4->offset = 5.0;
        offsetDef4->SetPlane(baseYOZ4);
        offsetPlane4->Create();

        // 2. Эскиз окружности
        ksEntityPtr circleSketch4 = Part1->NewEntity(o3d_sketch);
        ksSketchDefinitionPtr circleDef4 = circleSketch4->GetDefinition();
        circleDef4->SetPlane(offsetPlane4);
        circleSketch4->Create();

        ksDocument2DPtr circleDoc4 = circleDef4->BeginEdit();
        double centerY4 = -24.4;
        double centerZ4 = -25.0;
        double radius4 = 70.0 / 2.0;
        circleDoc4->ksCircle(centerY4, centerZ4, radius4, 1);
        circleDef4->EndEdit();

        // количество рёбер до создания тела
        ksEntityCollectionPtr allEdgesBefore = Part1->EntityCollection(o3d_edge);
        int edgeCountBefore = allEdgesBefore->GetCount();

        // 3. Вытягивание круга на 10 мм по X
        ksEntityPtr boss4 = Part1->NewEntity(o3d_bossExtrusion);
        ksBossExtrusionDefinitionPtr bossDef4 = boss4->GetDefinition();
        bossDef4->SetSketch(circleSketch4);
        bossDef4->directionType = dtNormal;
        bossDef4->SetSideParam(TRUE, etBlind, 10.0, 0.0, FALSE);
        boss4->Create();

        // Получаем новую коллекцию рёбер после создания тела
        ksEntityCollectionPtr allEdgesAfter = Part1->EntityCollection(o3d_edge);
        int edgeCountAfter = allEdgesAfter->GetCount();
        int newEdgesCount = edgeCountAfter - edgeCountBefore;

        // 4. Скругление только новых рёбер ===
        if (newEdgesCount > 0)
        {
            ksEntityPtr fillet4 = Part1->NewEntity(o3d_fillet);
            ksFilletDefinitionPtr filletDef4 = fillet4->GetDefinition();
            filletDef4->radius = 2.0;
            filletDef4->tangent = VARIANT_TRUE;

            ksEntityCollectionPtr filletEdges = filletDef4->array();
            int added = 0;

            // Перебор всех рёбер, относящихся к вытягиванию
            for (int i = edgeCountBefore; i < edgeCountAfter; ++i)
            {
                ksEntityPtr edge = allEdgesAfter->GetByIndex(i);
                if (edge)
                {
                    filletEdges->Add(edge);  // Добавляем ребро для скругления
                    added++;
                }
            }

            //часть кода для отладки
            /*if (added == 0)
            {
                kompas->ksMessage(_bstr_t(L"Не найдено рёбер для скругления"));
            }
            else
            {
                fillet4->Create();
                kompas->ksMessage(_bstr_t(L"Скруглено рёбер: ") + std::to_wstring(added).c_str());
            }*/
        }
        else
        {
            kompas->ksMessage(_bstr_t(L"Новых рёбер не найдено"));
        }


        
        // === вырез окружности Ø11 через все тела (YOZ, X=25) ===
        ksEntityPtr basePlaneYOZ3 = Part1->GetDefaultEntity(o3d_planeYOZ);

        // Создаём смещённую плоскость на 25 мм по X
        ksEntityPtr offsetPlane3 = Part1->NewEntity(o3d_planeOffset);
        ksPlaneOffsetDefinitionPtr offsetDef3 = offsetPlane3->GetDefinition();
        offsetDef3->direction = TRUE; // Вдоль положительного направления X
        offsetDef3->offset = 25.0;    // Смещение на 25 мм
        offsetDef3->SetPlane(basePlaneYOZ3);
        offsetPlane3->Create();

        // Создаём эскиз на новой плоскости
        ksEntityPtr circleSketch3 = Part1->NewEntity(o3d_sketch);
        ksSketchDefinitionPtr circleDef3 = circleSketch3->GetDefinition();
        circleDef3->SetPlane(offsetPlane3);
        circleSketch3->Create();

        ksDocument2DPtr circleDoc3 = circleDef3->BeginEdit();

        // Центр окружности тот же
        double circleCenterY3 = -24.4;
        double circleCenterZ3 = -25.0;     
        double circleRadius3 = 11.0 / 2.0; 

        circleDoc3->ksCircle(circleCenterY3, circleCenterZ3, circleRadius3, 1);
        circleDef3->EndEdit();

        // Выполняем вырезание на 35 мм назад (по -X)
        ksEntityPtr circleCut3 = Part1->NewEntity(o3d_cutExtrusion);
        ksCutExtrusionDefinitionPtr cutDef3 = circleCut3->GetDefinition();
        cutDef3->SetSketch(circleSketch3);
        cutDef3->directionType = dtNormal;
        cutDef3->SetSideParam(TRUE, etBlind, 35.0, 0.0, FALSE);
        circleCut3->Create();

        //для вариативного построения - добавление крепёжных элементов
        if (flag == 1) Additions(Part1);
        

        // ФИНАЛ
        // kompas->ksMessage(_bstr_t(L"опора построена"));

    }
    // перехват ошибок - например, для работы с СОМ интерфейсами
    catch (_com_error& e) {
        CString errorMsg;
        errorMsg.Format(_T("Ошибка COM: %s"), (LPCTSTR)e.Description());
        AfxMessageBox(errorMsg);
    }
    catch (...) {
        AfxMessageBox(_T("Неизвестная ошибка при построении детали"));
    }
}

//-------------------------------------------------------------------------------
// Головная функция библиотеки
// ---
void WINAPI LIBRARYENTRY(unsigned int comm)
{
    switch (comm)
    {
    case 1:
    {
        // Выдача сообщения пользователю
        // тестирование
            GetKompas();
            if (kompas) 
                kompas->ksMessage(_T("Тестовая команда 1"));

            break;
    }

    case 2:
        ShowKompasWindow();

        break;
    }
}



//-------------------------------------------------------------------------------
// Завершение приложения
// ---
void OnProcessDetach()
{
        
        kompas = NULL;
     
}
      



//------------------------------------------------------------------------------
// Загрузить строку из ресурса
// ---
CString LoadStr(int strID)
{
      
        TCHAR temp[_MAX_PATH];
    LoadString(LibraryDLL.hModule, strID, temp, _MAX_PATH);
    
        return temp;
}
