#include "PrimaryGeneratorActionMessenger.h"
#include "PrimaryGeneratorAction.h"

PrimaryGeneratorActionMessenger::PrimaryGeneratorActionMessenger(PrimaryGeneratorAction* primGeneratorAction)
    :fPrimGen(primGeneratorAction)
{

    fDirectory = new G4UIdirectory("/jpetmc/source/"); 
    fDirectory->SetGuidance("Commands for controling the gamma quanta source (beam/target/nema) and its parameters");

    fSourceType = new G4UIcmdWithAString("/jpetmc/source/setType",this);
    fSourceType->SetCandidates("beam isotope nema");
    fSourceType->SetDefaultValue("beam"); 

    fGammaBeamSetEnergy = new G4UIcmdWithADoubleAndUnit("/jpetmc/source/gammaBeam/setEnergy",this);
    fGammaBeamSetEnergy->SetGuidance("Set energy (value and unit) for beam of gamma quanta");
    fGammaBeamSetEnergy->SetDefaultValue(511);
    fGammaBeamSetEnergy->SetDefaultUnit("keV");
    fGammaBeamSetEnergy->SetUnitCandidates("keV");


    fGammaBeamSetPosition = new G4UIcmdWith3VectorAndUnit("/jpetmc/source/gammaBeam/setPosition",this);
    fGammaBeamSetPosition->SetGuidance("Set vertex position of the gamma quanta beam");
    fGammaBeamSetPosition->SetDefaultValue(G4ThreeVector(0,0,0));
    fGammaBeamSetPosition->SetDefaultUnit("cm");
    fGammaBeamSetPosition->SetUnitCandidates("cm");
    fGammaBeamSetPosition->SetParameterName("Xvalue","Yvalue","Zvalue",false);

    fGammaBeamSetMomentum = new G4UIcmdWith3VectorAndUnit("/jpetmc/source/gammaBeam/setMomentum",this);
    fGammaBeamSetMomentum->SetGuidance("Set momentum of the gamma quanta beam");
    fGammaBeamSetMomentum->SetDefaultValue(G4ThreeVector(1,0,0));
    fGammaBeamSetMomentum->SetDefaultUnit("keV");
    fGammaBeamSetMomentum->SetUnitCandidates("keV");
    fGammaBeamSetMomentum->SetParameterName("Xvalue","Yvalue","Zvalue",false);

    fIsotopeSetShape = new G4UIcmdWithAString("/jpetmc/source/isotope/setShape",this);
    fIsotopeSetShape->SetCandidates("cylinder");

    fIsotopeSetGenGammas = new G4UIcmdWithAnInteger("/jpetmc/source/isotope/setNGamma",this);
    fIsotopeSetGenGammas->SetGuidance("Give number of gamma quanta to generate 1 / 2 / 3 ");

    fIsotopeSetShapeDimCylinderRadius = new G4UIcmdWithADoubleAndUnit("/jpetmc/source/isotope/setShape/cylinderRadius",this);
    fIsotopeSetShapeDimCylinderRadius->SetGuidance("For cylindrical shape - set radius"); 
    fIsotopeSetShapeDimCylinderRadius->SetDefaultValue(10);
    fIsotopeSetShapeDimCylinderRadius->SetDefaultUnit("cm");
    fIsotopeSetShapeDimCylinderRadius->SetUnitCandidates("cm"); 

    fIsotopeSetShapeDimCylinderZ = new G4UIcmdWithADoubleAndUnit("/jpetmc/source/isotope/setShape/cylinderZ",this);
    fIsotopeSetShapeDimCylinderZ->SetGuidance("For cylindrical shape - set z (half length)"); 
    fIsotopeSetShapeDimCylinderZ->SetDefaultValue(10);
    fIsotopeSetShapeDimCylinderZ->SetDefaultUnit("cm");
    fIsotopeSetShapeDimCylinderZ->SetUnitCandidates("cm"); 

    fIsotopeSetCenter = new G4UIcmdWith3Vector("/jpetmc/source/isotope/setPosition",this);
    fIsotopeSetCenter->SetGuidance("Set position of the source");
    fIsotopeSetCenter->SetDefaultValue(G4ThreeVector(0,0,0));
    fIsotopeSetCenter->SetParameterName("Xvalue","Yvalue","Zvalue",false);

    fNemaPosition = new G4UIcmdWithAnInteger("/jpetmc/source/nema",this);
    fNemaPosition->SetGuidance("Give nema point number to simulate (1-6) ");
    fNemaPosition->SetDefaultValue(1);

    fCosmicOnly = new G4UIcmdWithoutParameter("/jpetmc/source/cosmiscOnly",this);
    fCosmicOnly->SetGuidance("Generate only cosmics");

}


PrimaryGeneratorActionMessenger::~PrimaryGeneratorActionMessenger()
{
    delete fIsotopeSetShape;
    delete fIsotopeSetShapeDimCylinderRadius;
    delete fIsotopeSetShapeDimCylinderZ;
    delete fSourceType;
    delete fGammaBeamSetEnergy;
    delete fGammaBeamSetPosition;
    delete fGammaBeamSetMomentum;
    delete fIsotopeSetCenter;
    delete fNemaPosition;
    delete fCosmicOnly;
}


void PrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if(command==fSourceType){
        fPrimGen->SetSourceTypeInfo(newValue);
    }

    if(command==fGammaBeamSetEnergy){
        CheckIfBeam();
        fPrimGen->GetBeamParams()->SetEnergy(fGammaBeamSetEnergy->GetNewDoubleRawValue(newValue));
    }

    if(command==fGammaBeamSetPosition){
        CheckIfBeam();
        fPrimGen->GetBeamParams()->SetVtxPosition(fGammaBeamSetPosition->GetNew3VectorValue(newValue));
    }

    if(command==fGammaBeamSetMomentum){
        CheckIfBeam();
        fPrimGen->GetBeamParams()->SetMomentum(fGammaBeamSetMomentum->GetNew3VectorValue(newValue));
    }


    if(command==fIsotopeSetShape){
        fPrimGen->GetIsotopeParams()->SetShape(newValue);
    }

    if(command==fIsotopeSetGenGammas){
        fPrimGen->GetIsotopeParams()->SetGammasNumber(fIsotopeSetGenGammas->GetNewIntValue(newValue));
    }


    if(command==fIsotopeSetShapeDimCylinderRadius){
        CheckIfIsotope();
        fPrimGen->GetIsotopeParams()->SetShapeDim(0,fIsotopeSetShapeDimCylinderRadius->GetNewDoubleRawValue(newValue));
    }


    if(command==fIsotopeSetShapeDimCylinderZ){
        CheckIfIsotope();
        fPrimGen->GetIsotopeParams()->SetShapeDim(1,fIsotopeSetShapeDimCylinderRadius->GetNewDoubleRawValue(newValue));
    }

    if(command==fIsotopeSetCenter){
        CheckIfIsotope();
        G4ThreeVector loc =  fIsotopeSetCenter->GetNew3VectorValue(newValue);
        fPrimGen->GetIsotopeParams()->SetShapeCenterPosition(
            loc.x(), loc.y(), loc.z()
        );
    }

    if(command==fNemaPosition){
        fPrimGen->SetSourceTypeInfo("nema");
	fPrimGen->SetNemaPoint(fNemaPosition->GetNewIntValue(newValue));
    }


    if(command==fCosmicOnly){
        fPrimGen->SetSourceTypeInfo("cosmic");
    }

}


void PrimaryGeneratorActionMessenger::CheckIfIsotope()
{
        if(fPrimGen->GetSourceTypeInfo() != "isotope")
        {
            G4Exception("PrimaryGeneratorActionMessenger","PGM02",JustWarning,
                    "Changed sourceType to isotope");
            fPrimGen->SetSourceTypeInfo("isotope");
        }

}

void PrimaryGeneratorActionMessenger::CheckIfBeam()
{
        // check if we really changing parameters corresponding to the beam
        if(fPrimGen->GetSourceTypeInfo() != "beam")
        {
            G4Exception("PrimaryGeneratorActionMessenger","PGM01",JustWarning,
                    "Changed sourceType to beam");
            fPrimGen->SetSourceTypeInfo("beam");
        }
}
