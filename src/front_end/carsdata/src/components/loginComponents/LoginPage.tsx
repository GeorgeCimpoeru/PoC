import LoginButton from '@/src/components/loginComponents/LoginButton';
import InputsLoginPage from '@/src/components/loginComponents//InputsLoginPage';
import GoToRegister from '@/src/components/loginComponents//GoToRegister';
import LoginTitle from '@/src/components/loginComponents//LoginTitle';

const LoginPage = () => {
    return (
        <section className="h-screen flex flex-col md:flex-row justify-center space-y-10 md:space-y-0 md:space-x-16 items-center my-2 mx-5 md:mx-0 md:my-0">
            <div className="md:w-1/3 max-w-sm flex flex-col items-center">
                <LoginTitle />
                <InputsLoginPage />
                <LoginButton />
                <GoToRegister />
            </div>
        </section>
    )
}

export default LoginPage
